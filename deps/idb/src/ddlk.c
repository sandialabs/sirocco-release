#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "idb.h"

#define SAMEDBH 0

/* A test program that creates a deadlock. */

size_t nthreads = 16;
size_t npertx = 8;
size_t ncomplete = 0;
size_t ntried = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

struct targs {
	IDB idb;
};

void *
dowork(void *vargs)
{
	struct targs *t;
	IDB idb;
	IDBX tx;
	struct idb_interval iv;
	size_t i, n;
	int rc, docommit;

	t = vargs;
#if SAMEDBH
	idb = t->idb;
#else
	(void)t;
	idb = idb_open("test", 0);
	if (idb == NULL)
		abort();
#endif

	n = 1;
	while(1) {
		//printf("Begin\n");
		docommit = 1;
		tx = idb_begin();
		if (tx == NULL)
			abort();

		for (i = 0; i < npertx; i++) {
			iv.vers = n;
			iv.rec = rand();
			iv.size = 1;
			iv.reclen = 1;
			iv.log_offset = 0;
			iv.epoch = 0;
			iv.flags = 0;
			//printf("Insert %lu\n", n);
			rc = idb_insert(tx, idb, &iv);
			//printf("...Done.\n");
			if (rc != 0 && rc != ECANCELED)
				abort();
			if (rc != 0) {
				docommit = 0;
			}
			n++;
		}

		//printf("Commit");
		rc = idb_end(tx, docommit);
		if (rc != 0)
			abort();
		pthread_mutex_lock(&m);
		if (docommit) {
			ncomplete++;
		}
		ntried++;
		pthread_mutex_unlock(&m);
		if (ntried > 0 && ntried % 100 == 0)
			printf("Completed %lu/%lu\n", ncomplete, ntried);
	}

	pthread_exit(NULL);
	return NULL;
}

int
main(int argc, char **argv)
{
	struct targs args;
	int rc;
	size_t i;
	pthread_t *t;

	(void)argc;
	(void)argv;

	t = malloc(sizeof(t[0]) * nthreads);
	if (t == NULL)
		abort();

	rc = idb_init();
	if (rc != 0)
		abort();

#if SAMEDBH
	args.idb = idb_open("test", 0);
	if (args.idb == NULL)
		abort();
#endif

	for (i = 0; i < nthreads; i++) {
		rc = pthread_create(t + i, NULL, dowork, (void *)(&args));
		if (rc != 0)
			abort();
	}

	for (i = 0; i < nthreads; i++) {
		rc = pthread_join(t[i], NULL);
		if (rc != 0)
			abort();
	}
	idb_close(args.idb);

	idb_fini();

	return 0;
}
