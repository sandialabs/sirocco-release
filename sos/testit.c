#define _POSIX_C_SOURCE 200112
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#include <aasg.h>
#include <ssm.h>
#include <sxm.h>

int debug = 0;

static char buf[1000000];
static char rbuf[1000000];

void usage(void);
static void errprnt(const char *s, int errnum);

static void merror(char *name, long rv, char *arg) {
  if (errno == ERANGE) {
    fprintf(stderr, "%s error: %s\n", name, strerror(errno));
    exit(-1);
  } else if (rv < 0) {
    fprintf(stderr, "%s out of range: %s\n", name, arg);
    exit(-1);
  }
}

static int ts_diff(struct timespec *start, 
		   struct timespec *end,
		   struct timespec *out) {
  if (end->tv_sec < start->tv_sec)
    return -1;
  if (end->tv_nsec < start->tv_nsec) {
    out->tv_nsec = start->tv_nsec - end->tv_nsec;
    end->tv_sec -= 1;
  } else {
    out->tv_nsec = end->tv_nsec - start->tv_nsec;
  }
  out->tv_sec = end->tv_sec - start->tv_sec;
  return 0;
}


int
main(int argc, char *const argv[])
{
	int	i, err;
	ssm_Iaddr iaddr;
	ssm_Haddr raddr;
	struct xm_transport *xmx;
	ssmptcp_addrargs_t addrargs;
	asg_transport_t transport;
	asg_location_t location;
	asg_record_id_t rec;
	asg_size_t nbytes, count[2], bufsiz, write, written;
	struct timespec start, end, diff;
	asg_update_id_t vers[2];
	long rv, port, lp;
	char *addr;

	bufsiz = write = port = lp = 0;
	addr = NULL;

	if (argc <= 6) {
		printf("optind: %d\nargc: %d\n", optind, argc);
		usage();
	}


	while ((i = getopt(argc, argv, "db:w:a:p:l:")) != -1) {
		switch (i) {
		case 'd':
			debug = 1;
			break;
		case 'b':
			rv = strtol(optarg, NULL, 10);
			merror("buffer size", rv, optarg);
			bufsiz = (asg_size_t)rv;
			//set buffer size
			break;
		case 'w':
			rv = strtol(optarg, NULL, 10);
			merror("buffer size", rv, optarg);
			write = (asg_size_t)(rv*(1024*1024));
			//set total Mbytes written
			break;
		case 'p':
			rv = strtol(optarg, NULL, 10);
			merror("port", rv, optarg);
			port = rv;
			break;
		case 'a':
			addr = optarg;
			break;
		case 'l':
			rv = strtol(optarg, NULL, 10);
			merror("listening port", rv, optarg);
			lp = rv;
			break;
		default:
			usage();
		}
	}

	if (bufsiz == 0 ||
	    write == 0 ||
	    port == 0 ||
	    lp == 0 ||
	    addr == NULL) {
		printf("invalid arguments\n");
		usage();
	}

	do {
		if ((err = xmtcp_start(&xmx, lp)) ||
		    (err = aasg_create_transport(xmx, &transport)))
			break;
		addrargs.host = addr;
		addrargs.port = port;
		iaddr = ssm_addr(xmx->ssm);
		raddr = iaddr->create(iaddr, &addrargs);
		
		if ((err = aasg_create_location(transport,
						raddr,
						&location)) != 0) {
			break;
		}
	} while (0);
	if (err) {
	        errprnt(argv[optind], -err);
		exit(EXIT_FAILURE);
	}

	rec = 0;
	nbytes = sizeof(buf);
	
	if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
	  fprintf(stderr, "clock_gettime\n");
	  exit(-1);
	}

	written = 0;
	while (written < write) {
		struct asg_session ses;
		struct asg_id id;
		struct asg_batch_id_t *b;

		err = asg_batch_init(&b, 0,
				     location, NULL, NULL);

		ses.inst = 0;
		ses.loc = location;
		ses.batch = b;

		id.c = 0;
		id.o = 0;
		id.f = 0;
		id.r = rec;
		id.rcount = bufsiz;

		err =
		    aasg_write(ses,
			       id,
			       1,
			       ASG_COND_NONE,
			       0,
			       &vers[0],
			       buf,
			       &count[0]);
		if (err) {
			errprnt(argv[optind], -err);
			exit(EXIT_FAILURE);
		}

		err = 
		    aasg_read(ses,  
			      id, 
			      ASG_COND_NONE, 
			      0, 
			      rbuf, 
			      bufsiz, 
			      &count[1], 
			      NULL, 
			      0, 
			      NULL, 
			      &vers[1]);

		if (err) {
			errprnt(argv[optind], -err);
			exit(EXIT_FAILURE);
			}

		err = asg_batch_submit(b);

		if (err) {
			errprnt(argv[optind], -err);
			exit(EXIT_FAILURE);
		}

		if (count[0] != bufsiz) {
			printf("write count: %lu\n", count[0]);
			(void )fprintf(stderr, "xfer count mismatch\n");
			exit(EXIT_FAILURE);
		}

		if (count[1] != bufsiz) {
			printf("read count: %lu\n", count[1]);
			(void )fprintf(stderr, "xfer count mismatch\n");
			exit(EXIT_FAILURE);
		}

		rec += nbytes;
		written += bufsiz;
	}
	if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
	  fprintf(stderr, "clock_gettime\n");
	  exit(-1);
	}

	if (ts_diff(&start, &end, &diff) != 0) {
	  fprintf(stderr, "start and end times are inconsistent\n");
	} else {
	  printf("Bytes\tSec\tNsec\n"
		 "%lu\t%lu\t%lu\n", written, diff.tv_sec, diff.tv_nsec);
	}
	ssm_addr_destroy(xmx->ssm, raddr);
	if (aasg_destroy_location(&location))
		fprintf(stderr, "location unable to be destroyed\n");

	
	if (aasg_destroy_transport(&transport))
		fprintf(stderr, "transport unable to be destroyed\n");

	xm_stop(xmx);

	return EXIT_SUCCESS;
}

void
usage()
{

	(void )fprintf(stderr, "Usage: %s [-d]\n"
		       "-b [buffer size]\n"
		       "-w [bytes to write]\n"
		       "-a [remote address, dotted quad]\n"
		       "-p [remote port]\n"
		       "-l [listening port]\n",
		       "testit");
	(void )fflush(stderr);
	exit(EXIT_FAILURE);
}

static void
errprnt(const char *s, int errnum)
{

	(void )fprintf(stderr, "testit %s: %s\n", s, strerror(errnum));
	(void )fflush(stderr);
}
