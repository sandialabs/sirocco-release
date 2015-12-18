/* idb.c
 *
 * IDB benchmark/stress test
 *
 * Originally written by Lee Ward
 * Enhanced by Geoff Danielson
 * Bloated by Matthew Curry
 */

#define _XOPEN_SOURCE	500

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "idb.h"

#define BAIL() _bail(__FILE__, __LINE__)
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define INSERT 0
#define REMOVE 1

static int main_argc;
static char *const *main_argv;

#ifdef USE_AESOP
#include "aeidb.hae"
#define AENAME(x) ae ## x
#define MAYBLOCK __blocking
#define AE_PBRANCH pbranch
#define AE_PWAIT pwait
#define IDB_FN(x) aeidb_ ## x
#else
#define AENAME(x) x
#define MAYBLOCK
#define AE_PBRANCH
#define AE_PWAIT
#define IDB_FN(x) idb_ ## x
#endif

static int verbose = 0;

static unsigned seed;

/* Workload configuration */
static int seq = 0;
static unsigned iratio = 100;
static idb_off_t max_recs = IDB_OFF_MAX;
static unsigned long numops = ULONG_MAX;
static long aesop_pb = 1;
static long ncontexts = 1;

/* Correctness/consistency checking */
static int test_log_offset = 0;
static idb_size_t log_offset_c = 37;
static int fastmap = 0;
static int usemap = 0;
static size_t nentries = IDB_OFF_MAX;
static size_t mapsize = 0;            /* Size of map, in bytes */
static idb_off_t mapskip = 1;            /* Updates between checks of map */


/* IDB tuning */
static idb_flags_t idb_flags = 0;


static pthread_mutex_t rand_m = PTHREAD_MUTEX_INITIALIZER;
struct mentry {
	idb_vers_t vers;
	idb_size_t reclen;            /* If zero, not instantiated */
};

struct check_map_args {
	idb_off_t last;
	struct mentry *map;
};

static void usage(void);
static void _bail(const char *file, int line);
static int parseargs(int argc, char *const argv[]);
static int parseul(const char *s, unsigned long *ulp);
static struct idb_interval *genrgn(struct idb_interval *iv,
				   idb_off_t rmax,
				   idb_size_t smax,
				   idb_size_t lmax,
				   idb_size_t fmax);
MAYBLOCK static int check_log_offset(const struct idb_interval *, void *);
MAYBLOCK static int check_map_scb(const struct idb_interval *, void *);
MAYBLOCK static int check_map(const struct idb_interval *, struct mentry *, IDBX,
			       IDB);
static void set_map(const struct idb_interval *, struct mentry *, int);

struct testargs {
	unsigned tid;
	IDBC c;
	pthread_t t;
};

static void
init_testargs(struct testargs *ta, unsigned tid, IDBC c)
{
	ta->tid = tid;
	ta->c = c;
}

static MAYBLOCK void *
runtest(void *arg)
{
	IDB	idb;                     /* database handle    */
	IDBX	idbx;                    /* transaction handle */
	int err, op;
	struct mentry *map;              /* debug map */
	char dbname[9];
	struct idb_interval interval;    /* test interval      */
	long j;
	time_t t, dt;
	struct testargs *ta;
	int modified;                    /* Indicates whether map is
					    consistent for pre-op check */

	ta = arg;
	modified = 0;
	t = time(NULL);
	map = NULL;
	if (usemap) {
		map = calloc(nentries, sizeof(struct mentry));
		if (map == NULL) {
			(void )fprintf(stderr, "Map too large, try something smaller.\n");
			exit(1);
		}
	}
	pthread_mutex_lock(&rand_m);
	for (j = 0; j < (long)sizeof(dbname)-1; j++)
		dbname[j] = (char)(rand() % 26 + 'a');
	pthread_mutex_unlock(&rand_m);
	dbname[j] = '\0';

	/* create test database */
	do {
		idb = idb_open(dbname, 0, ta->c);
		if (idb == NULL && errno != IDB_DEADLOCK) {
			(void )fprintf(stderr,
				       "Cannot open database for %s: %s\n",
				       dbname,
				       strerror(errno));
			return NULL;
		}
	} while (errno == IDB_DEADLOCK);

	/* perform insert/remove operations on test database */
	j = 0;
	interval.rec = -1;
	while (j < numops / aesop_pb / ncontexts) {
		if (verbose >= 2) {
			if ((dt = time(NULL) - t) >= 15) {
				t += dt;
				printf("%lu/%lu\n", j, numops/aesop_pb/ncontexts);
			}
		}
		err = 0;
		/* begin transaction */
		idbx = IDB_FN(begin)(ta->c);
		if (idbx == NULL) {
			(void )fprintf(stderr,
				       "Cannot begin transaction: %s\n",
				       strerror(errno));
			abort();
		}

		/* generate fresh random test interval */
		if (modified == 0)
			(void )genrgn(&interval, max_recs-1, 64*1024,
				      IDB_SIZE_MAX, IDB_SIZE_MAX);

		if (modified == 0 && j % mapskip == 0) {
			err = check_map(&interval, map, idbx, idb);
			if (err != 0 && errno == IDB_DEADLOCK) {
				err = IDB_FN(end)(idbx, 0);
				if (err)
					abort();
				continue;
			}
			if (err != 0)
				abort();
		}

		/* perform insert or remove on test interval */
		op = (random() % 100 < iratio) ? INSERT : REMOVE;
		if (op == INSERT) {
			err = IDB_FN(insert)(idbx, idb, &interval);
		} else {
			err = IDB_FN(remove)(idbx, idb, &interval);
		}
		if (err != 0 && errno == IDB_DEADLOCK) {
			err = IDB_FN(end)(idbx, 0);
			if (err)
				abort();
			continue;
		}
		if (err != 0) {
			err = errno;
			(void )fprintf(stderr, "Cannot %s region: %s\n",
				       (op == INSERT) ? "insert" : "remove",
				       strerror(err));
			abort();
		}
		set_map(&interval, map, op);
		modified = 1;
		if (j % mapskip == 0) {
			err = check_map(&interval, map, idbx, idb);
			if (err != 0 && errno == IDB_DEADLOCK) {
				err = IDB_FN(end)(idbx, 0);
				if (err)
					abort();
				continue;
			}
			if (err != 0)
				abort();
		}

		/* end transaction, committing results to disk */
		err = IDB_FN(end)(idbx, !err ? 1 : 0);
		if (err != 0 && errno == IDB_DEADLOCK) {
			/* IDB auto-aborts if a commit fails, even for
			   deadlock. */
			continue;
		}

		if (err != 0) {
			(void )fprintf(stderr,
				       "End transaction failed: %s\n",
				       strerror(errno));
			abort();
		}
		modified = 0;
		j++;
	}
	if (verbose >= 2) {
		printf("%lu/%lu\n", j, numops/aesop_pb/ncontexts);
	}

	if (idb_close(idb) != 0) {
		(void )fprintf(stderr,
			       "While closing database for %s: %s\n",
			       dbname,
			       strerror(errno));
		return NULL;
	}

	if (usemap)
		free(map);
	return NULL;
}

/** Creates a test database by performing specified number
 *  of random insert/remove operations following a distribution **/
MAYBLOCK int
AENAME(main)(int argc, char **argv)
{
	int	err;                     /* error state        */
	long i, j;
	IDBC c;
	struct testargs *test_args;
	char p[16];

	main_argc = argc;
	main_argv = argv;

	seed = (unsigned )time(NULL);

	if (parseargs(argc, argv))
		return 1;

	if (verbose >= 1) {
		(void )fprintf(stdout, "max_recs = %lu\n", max_recs);
		(void )fprintf(stdout, "Seed is %u\n", seed);
	}
	srandom(seed);

	test_args = malloc(sizeof(*test_args) * aesop_pb * ncontexts);
	if (test_args == NULL) {
		perror("malloc");
		abort();
	}

	for (i = 0; i < ncontexts; i++) {
		err = snprintf(p, sizeof(p), "%x", (unsigned)i);
		if (err < 0 || err >= sizeof(p))
			BAIL();
		err = mkdir(p, 0755);
		if (err != 0 && errno != EEXIST) {
			perror("mkdir");
			BAIL();
		}
		c = idb_init(p, idb_flags);
		if (c == NULL) {
			perror("idb_init");
			abort();
		}
		for (j = 0; j < aesop_pb; j++) {
			init_testargs(test_args + i*aesop_pb + j, (unsigned)i, c);
		}
	}

#ifdef USE_AESOP
	AE_PWAIT {
		for (i = 0; i < aesop_pb * ncontexts; i++) {
			AE_PBRANCH {
				runtest((void *)(test_args + i));
			}
		}
	}
	err = 0;
#else
	/* Launch threads! */
	for (i = 0; i < aesop_pb * ncontexts; i++) {
		err = pthread_create(&test_args[i].t, NULL, runtest,
				     (void *)(test_args + i));
		if (err != 0) {
			fprintf(stderr, "pthread_create: %s\n", strerror(err));
			exit(1);
		}
	}
	for (i = 0; i < aesop_pb * ncontexts; i++) {
		err = pthread_join(test_args[i].t, NULL);
		if (err != 0) {
			fprintf(stderr, "pthread_join: %s\n", strerror(err));
			exit(1);
		}
		assert(err == 0);
	}
#endif

	for (i = 0; i < ncontexts; i++) {
		idb_fini(test_args[i*aesop_pb].c);
	}
	/** print cache statistics **/
	if (verbose >= 1) {
		(void )fprintf(stderr,
			       "# gets %llu, # hits %llu, ratio %f\n",
			       _idb_statistics.ngets,
			       _idb_statistics.nhits,
			       (((double )_idb_statistics.nhits /
			         (double )_idb_statistics.ngets) *
			        100.0));
	}
	free(test_args);
	return 0;
}

#ifdef USE_AESOP
aesop_main_set(aemain);
#endif

/** prints usage for program with arguments **/
static void
usage(void)
{
	fprintf(stderr, "Usage: %s [OPTIONS] ...\n", main_argv[0]);
	fprintf(stderr,
"Verbosity\n\
   -d    Enable libidb's internal debugging output. Use multiple times for\n\
         more\n\
   -h    Print this usage statement\n\
   -v    Enable verbose output for the test. Use multiple times for more.\n\
\n\
Workload configuration\n\
   -C    Number of IDB contexts to use, each with its own subdirectory\n\
   -I    Insert ratio, between 1 and 100\n\
   -P    Number of parallel IDB updates per context\n\
   -s    Generate sequential, non-coelescing workloads (default=random)\n\
   -T    Time to run, default=no limit\n\
   -U    Number of updates to run, default=ULONG_MAX\n\
   -X    Maximum range of records used, default=no limit\n\
   -0    Does not modify behavior in any way\n\
\n\
Correctness/consistency checking\n\
   -N    Number of iterations between consistency checks incurred by by -Z\n\
         and -Y.\n\
   -o    Test log offset updates\n\
   -R    Set seed for random number generator\n\
   -Y    Use a map of <bytes> size to check the database less slowly\n\
         (requires empty IDBs, implies -X)\n\
   -Z    Use a map of <bytes> size to check the database more slowly\n\
         (requires empty IDBs, implies -X)\n\
\n\
IDB tuning\n\
   -g    Use a shared table for IDBs (default=non-shared)\n\
   -m    Use multiversion/snapshot isolation (default=rwlocks)\n\
");
	exit(1);
}

static void
arg_error(const char *pdesc, char opt, char *val)
{
	(void )fprintf(stderr, "Bad value for %s (-%c): %s\n",
		       pdesc, opt, val);
	(void )fprintf(stderr, "Try %s -h for more information.\n",
		       main_argv[0]);
	exit(1);
}

/** parses command line args, returning index of first non-option arg **/
static int
parseargs(int argc, char *const argv[])
{
	int	i, err = 0;
	unsigned long ul;

	while ((i = getopt(argc, argv, ":dghmosv0"
			   "C:I:N:P:R:T:U:X:Y:Z:")) != -1) {
		switch (i) {
		case 'd':
			_idb_debug++;
			break;
		case 'h':
			usage();
			err = 1;
			break;
		case 'v':
			verbose++;
			break;
		case 'C':
			if (parseul(optarg, &ul) != 0 || !ul)
				arg_error("number of contexts", i, optarg);
			ncontexts = ul;
			break;
		case 'I':
			if (parseul(optarg, &ul) != 0 || ul > 100)
				arg_error("insert ratio", i, optarg);
			iratio = (unsigned )ul;
			break;
		case 'P':
			if (parseul(optarg, &ul) != 0 || !ul)
				arg_error("number of parallel updates", i,
					  optarg);
			aesop_pb = ul;
			break;
		case 's':
			seq = 1;
			break;
		case 'T':
			fprintf(stderr, "Unsupported\n");
			abort();
			break;
		case 'U':
			if (parseul(optarg, &ul) != 0)
				arg_error("total number of updates", i,
					  optarg);
			numops = ul;
			break;
		case 'X':
			if (parseul(optarg, &ul) != 0 || !ul)
				arg_error("maximum range of records", i,
					  optarg);
			max_recs = ul;
			break;
		case 'N':
			if (parseul(optarg, &ul) != 0 || !ul)
				arg_error("skipped ops between checks", i,
					  optarg);
			mapskip = ul;
			break;
		case 'o':
			test_log_offset = 1;
			break;
		case 'R':
			if (parseul(optarg, &ul) != 0)
				arg_error("random number generator seed", i,
					  optarg);
			seed = ul;
			break;
		case 'Y':
			fastmap = 1;
			/* Fall through */
		case 'Z':
			usemap = 1;
			if (parseul(optarg, &ul) != 0 ||
			    ul < sizeof(struct mentry)) {
				fprintf(stderr, "Hint: Try multiples of "
					"%lu bytes\n",
					sizeof(struct mentry));
				arg_error("map size", i, optarg);
			}
			mapsize = ul;
			nentries = mapsize / sizeof(struct mentry);
			break;
		case 'g':
			idb_flags |= IDB_SHARED_TABLE;
			break;
		case 'm':
			idb_flags |= IDB_USE_SNAPSHOT;
			break;
		case '0':
			break;
		case '?':
			fprintf(stderr, "Invalid option -%c\n", optopt);
			fprintf(stderr, "Try %s -h for more information\n",
				main_argv[0]);
			err = -1;
			break;
		case ':':
			fprintf(stderr, "Option -%c missing argument\n",
				optopt);
			fprintf(stderr, "Try %s -h for more information\n",
				main_argv[0]);
			break;
		default:
			BAIL();
		}
	}

	if (usemap && max_recs != IDB_OFF_MAX && max_recs < nentries) {
		fprintf(stderr, "Maximum records set to %lu, but is limited "
			"to %lu by map size. Exiting.\n", max_recs, nentries);
		exit(1);
	}
	if (max_recs == IDB_OFF_MAX && usemap) {
		max_recs = nentries;
	}

	if (argc - optind) {
		fprintf(stderr, "%s is not a valid option.\n",
			main_argv[optind]);
		fprintf(stderr, "Try %s -h for more information.\n",
			main_argv[0]);
	}

	return (argc - optind) || err;
}

/** parses unsigned long int from input string
 *
 * @param s   input string
 * @param ulp stores result of parse
 *
 * @return 0 on success, -1 on error
 */
static int
parseul(const char *s, unsigned long *ulp)
{
	unsigned long ul;
	char	*end;

	ul = strtoul(s, &end, 0);
	if (!(*s != '\0' && *end == '\0') ||
	    (ul == ULONG_MAX && errno == ERANGE))
		return -1;
	*ulp = ul;
	return 0;
}

static uint64_t
bigrandom()
{
	uint64_t ret = 0;
	int i;

	pthread_mutex_lock(&rand_m);
	for (i = 0; i < sizeof(ret); i++)
		ret = (ret << 8) + (uint64_t)(random());
	pthread_mutex_unlock(&rand_m);

	return ret;
}

static int64_t
iabs(int64_t i)
{
	if (i < 0)
		return i * -1;
	return i;
}

/** updates given interval with randomly chosen field values
 *
 *  @param iv   interval to update
 *  @param rmax upper bound for logical offsets
 *  @param smax upper bound for logical interval size
 *  @param lmax upper bound for physical offsets
 *  @param fmax upper bound for transfer mode flags
 *
 *  @return address of interval iv
 */
static struct idb_interval *
genrgn(struct idb_interval *iv, idb_off_t rmax, idb_size_t smax,
       idb_size_t lmax, idb_size_t fmax)
{
	idb_off_t a, b, tmp;

	iv->vers = 0;

	/* check that rmax is within sane range */
	assert(rmax >= 0);

	if (seq == 0) {
		/* generate random offset range */
		a = iabs((idb_off_t )bigrandom()) % rmax;         /* logical offset start */
		do {
			b = iabs((idb_off_t )bigrandom()) % rmax; /* logical offset end */
		} while (a == b);
	} else {
		if (iv->rec + 2 > rmax)
			iv->rec = -1;
		a = iv->rec+1;
		b = iv->rec+2;
	}
	if (b < a) {
		tmp = a;
		a = b;
		b = tmp;
	}
	iv->rec = a;

	/* calculate extent from offsets, and sanity check */
	iv->size = (idb_size_t )(b - a);
	if (iv->size > smax)
		iv->size = (iv->size % (smax - 1)) + 1;

	/* set epoch to current */
	iv->epoch = (idb_size_t )time(NULL);
	if (test_log_offset)
		iv->reclen = (idb_size_t)(random() % 32);
	else
		iv->reclen = 1;

	iv->log_offset = (idb_size_t)iv->rec * iv->reclen + log_offset_c;

	/* generate random physical offset, flags, and version */
	iv->flags = (idb_size_t )random() % fmax;
	iv->vers = (idb_size_t )random() % lmax;

	memset(iv->data, '\0', IDB_UDSZ);

	return iv;
}

MAYBLOCK static int
check_log_offset(const struct idb_interval *iv, void *arg)
{

	(void)arg;
	if (iv->log_offset != (idb_size_t)iv->rec * iv->reclen + log_offset_c)
		BAIL();
	return 1;
}

static void
set_map(const struct idb_interval *iv, struct mentry *m, int op)
{
	size_t i;

	if (usemap == 0)
		return;

	for (i = iv->rec; i < iv->rec + iv->size; i++) {
		m[i].vers = iv->vers;
		m[i].reclen = (op == INSERT) ? iv->reclen : 0;
	}
}

MAYBLOCK static int
check_map(const struct idb_interval *iv, struct mentry *m, IDBX idbx, IDB idb)
{
	struct idb_interval small_civ, big_civ, *civ;
	struct check_map_args cma;
	int rc;
	size_t i;

	if (usemap == 0)
		return 0;

	small_civ = *iv;
	if (small_civ.rec > 0) {
		small_civ.rec--;
		small_civ.size++;
	}
	if (small_civ.rec + small_civ.size < IDB_OFF_MAX) {
		small_civ.size++;
	}
	big_civ.rec = 0;
	big_civ.size = IDB_OFF_MAX;


	if (test_log_offset) {
		rc = IDB_FN(stab)(idbx, idb, &small_civ, check_log_offset,
				  NULL);
		if (rc) {
			if (errno != IDB_DEADLOCK) {
				rc = errno;
				perror("stab");
				errno = rc;
			}
			return -1;
		}
	}

	if (fastmap)
		civ = &small_civ;
	else
		civ = &big_civ;
	cma.last = civ->rec;
	cma.map = m;

	rc = IDB_FN(stab)(idbx, idb, civ, check_map_scb, &cma);
	if (rc) {
		if (errno != IDB_DEADLOCK) {
			rc = errno;
			perror("stab");
			errno = rc;
		}
		return -1;
	}
	/* If the last index seen is within the check interval, ensure
	 * that the rest of the interval doesn't exist */
	civ->size = MIN(civ->size, nentries);
	for (i = cma.last; i < civ->rec + civ->size; i++) {
		if (m[i].reclen != 0) {
			abort();
		}
	}

	return 0;
}

MAYBLOCK static int
check_map_scb(const struct idb_interval *iv, void *arg)
{
	struct check_map_args *cma = arg;
	idb_off_t *last, i;
	struct mentry *map;

	last = &(cma->last);
	map = cma->map;

	assert(*last <= iv->rec || fastmap);
	if (fastmap && *last > iv->rec)
		*last = iv->rec;

	for (i = *last; i < iv->rec; i++) {
		if (map[i].reclen != 0)
			BAIL();
	}
	for (i = iv->rec; i < iv->rec + (idb_off_t)iv->size; i++) {
		if (map[i].vers != iv->vers ||
		    map[i].reclen != iv->reclen)
			BAIL();
	}
	*last = iv->rec + (idb_off_t)iv->size;
	return 1;
}

static void
_bail(const char *file, int line)
{
	size_t i;

	for (i = 0; i < (size_t)main_argc; i++)
		printf("%s ", main_argv[i]);
	fprintf(stderr, "failed at %s:%i with seed %i\n", file, line, seed);
	assert(0);
	abort();
}

