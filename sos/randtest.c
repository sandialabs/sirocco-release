/* -*- C -*- */
/* testrand.c: a test program utilizing libsos to write random segments
 * of a fixed buffer size to an asg store, and then probe and retrieve 
 * them.
 *
 * File Author: Geoff Danielson, gcdanie@sandia.gov
 * File Maintainer: Geoff Danielson, gcdanie@sandia.gov
 */

#define _POSIX_C_SOURCE 200112
#define _DEFAULT_SOURCE

#define MAXBUF 1000000

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

char buf[MAXBUF];
char chkbuf[MAXBUF];

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

int
main(int argc, char *const argv[])
{
	/* transport startup variables */
	ssm_Iaddr iaddr;
	ssm_Haddr raddr;
	struct xm_transport *xmx;
	ssmptcp_addrargs_t addrargs;

	/* ASG variables */
	asg_transport_t transport;
	asg_location_t location;
	asg_size_t count, bufsiz, write, written, read, probed;
	asg_update_id_t vers;
	asg_record_info_t probe;
	struct asg_session ses;
	struct asg_id id;
	struct asg_batch_id_t *b;

	/* housekeeping variables */
	long rv, port, lp;
	char *addr;
	size_t *map, i, j, index, records, rerrors, prerrors;
	int err;
		
	bufsiz = write = port = lp = 0;
	addr = NULL;

	if (argc <= 6) {
		printf("optind: %d\nargc: %d\n", optind, argc);
		usage();
	}

	while ((err = getopt(argc, argv, "db:w:a:p:l:")) != -1) {
		switch (err) {
		case 'd':
			debug = 1;
			break;
		case 'b':
			/* set buffer size */
			rv = strtol(optarg, NULL, 10);
			merror("buffer size", rv, optarg);
			bufsiz = (asg_size_t)rv;
			if (bufsiz > MAXBUF) {
				(void )fprintf(stderr, "bufsize out of range\n");
				exit(EXIT_FAILURE);
			}
			break;
		case 'w':
			/* set total MiB to write */
			rv = strtol(optarg, NULL, 10);
			merror("buffer size", rv, optarg);
			write = (asg_size_t)(rv*(1024*1024));
			break;
		case 'p':
			/* set remote port to which to connect */
			rv = strtol(optarg, NULL, 10);
			merror("port", rv, optarg);
			port = rv;
			break;
		case 'a':
			/* set remote IP to which to connect */
			addr = optarg;
			break;
		case 'l':
			/* set local port on which to listen */
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

	records = write/bufsiz;
	if (write%bufsiz > 0)
		records += 1;

	(void )fprintf(stderr, "Writing, reading, and probing %lu records\n"
		       "Buffer: %lu\nBytes: %lu\n",
		       records, bufsiz, records * bufsiz);

	/* create the ASG transport locally */
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

	map = malloc(sizeof(size_t)*(records));

	if (map == NULL) {
		fprintf(stderr, "Out of memory\n");
		exit(-1);
	}

	for (i = 0; i < records; i++) {
		map[i] = i;
	}

	srand((unsigned)(time(NULL)));

	/* roll up an ASG ID to write to */
	id.c = (asg_container_id_t )rand();
	id.o = (asg_object_id_t )rand();
	id.f = (asg_fork_id_t )rand();
	
	/* shuffle the map */
	if (debug) {
		(void )fprintf(stderr, "first shuffle\n");
	}
	for (i = 0; i < records; i++) {
		index = ((size_t )rand())%records;
		j = map[i];
		map[i] = map[index];
		map[index] = j;
	}

	if (debug) {
		for (i = 0; i < records; i++) {
			(void )fprintf(stderr, "map[%lu] = %lu\n", i, map[i]);
		}
		
	}

	/* write cycle */
	written = 0;

	for (i = 0; i < records; i++) {
		/* seed the random with the map ID */
		srand(map[i]);

		/* populate the buffer with some reproducible gibberish */
		for (j = 0; j < bufsiz; j++) {
			sprintf(&buf[j], "%x", rand()%16); 
		}

		/* using the asynchronous interface here, we must initialize
		 * a batch beforehand.
		 */

		err = asg_batch_init(&b,          /* ASG batch id */
				     0,           /* ASG instance, not used */
				     location,    /* ASG location */
				     NULL,        /* callback function */
				     NULL);       /* callback data */

		if (err) {
			(void )fprintf(stderr, "asg_batch_init failed: %s\n",
				       strerror(err));
			free(map);
			exit(EXIT_FAILURE);
		}

		ses.inst = 0;                      /* instance (not used) */
		ses.loc = location;                /* ASG location */
		ses.batch = b;                     /* ASG batch id */

		id.r = map[i];                     /* record offset */
		id.rcount = 1;                     /* record count */

		err =
			aasg_write(ses,            /* ASG session */
				   id,             /* ASG record id */
				   bufsiz,         /* record length */
				   ASG_AUTO_UPDATE_ID, /* ASG flags */
				   0,              /* conditional version */
				   &vers,          /* new version */
				   buf,            /* buffer */
				   &count);        /* records written */
		if (err) {
			(void )fprintf(stderr, "aasg_write failed: %s\n",
				       strerror(err));
			free(map);
			exit(EXIT_FAILURE);
		}

		/* submitting the batch with the synchronous function 
		 * will block until completion 
		 */

		err = asg_batch_submit(b);

		if (err) {
			(void )fprintf(stderr, 
				       "asg_batch_submit failed: %s\n",
				       strerror(err));
			free(map);
			exit(EXIT_FAILURE);
		}

		if (count != bufsiz) {
			printf("write count: %lu\n", count);
			(void )fprintf(stderr, "xfer count mismatch\n");
			free(map);
			exit(EXIT_FAILURE);
		}

		written += bufsiz;
	}

	fprintf(stderr, "Wrote %lu\n", written);

	/* reshuffle the map */
	if (debug) {
		fprintf(stderr, "reshuffling map\n");
	}

	for (i = 0; i < records; i++) {
		index = ((size_t )rand())%records;
		j = map[i];
		map[i] = map[index];
		map[index] = j;
	}

	if (debug) {
		for (i = 0; i < records; i++) {
			(void )fprintf(stderr, "map[%lu] = %lu\n", i, map[i]);
		}
	}

	/* read/probe cycle */

	read = rerrors = prerrors = 0;

	for (i = 0; i < records; i++) {
		/* reseed the PRN with the mapid */
		srand(map[i]);

		/* reproduce gibberish for check */
		memset(chkbuf, 0, MAXBUF);
		memset(buf, 0, MAXBUF);
		for (j = 0; j < bufsiz; j++) {
			sprintf(&chkbuf[j], "%x", rand()%16); 
		}

		err = asg_batch_init(&b,          /* ASG batch id */
				     0,           /* ASG instance, not used */
				     location,    /* ASG location */
				     NULL,        /* callback function */
				     NULL);       /* callback data */

		if (err) {
			(void )fprintf(stderr, "asg_batch_init failed: %s\n",
				       strerror(err));
			free(map);
			exit(EXIT_FAILURE);
		}

		ses.inst = 0;                      /* instance (not used) */
		ses.loc = location;                /* ASG location */
		ses.batch = b;                     /* ASG batch id */

		id.r = map[i];                     /* record offset */
		id.rcount = 1;                     /* record count */

		
		err = 
			aasg_read(ses,             /* ASG session */
				  id,              /* ASG record id */
				  ASG_COND_NONE,   /* ASG flags */
				  0,               /* ASG flags conditional */
				  buf,             /* buffer */
				  bufsiz,          /* bufsize */
				  &count,          /* count of records read */
				  &probe,          /* ASG record probe */
				  sizeof(asg_record_info_t), /* rec bufsize */
				  &probed,         /* count of probes */
				  &vers);          /* version return */

		if (err) {
			(void )fprintf(stderr, "aasg_read failed: %s\n",
				       strerror(err));
			free(map);
			exit(EXIT_FAILURE);
		}

		err = asg_batch_submit(b);

		if (err) {
			(void )fprintf(stderr, 
				       "asg_batch_submit failed: %s\n",
				       strerror(err));
			free(map);
			exit(EXIT_FAILURE);
		}

		if (count != bufsiz) {
			printf("read count: %lu\n", count);
			(void )fprintf(stderr, "xfer count mismatch\n");
			free(map);
			exit(EXIT_FAILURE);
		}

		if (probed != 1) {
			printf("probe count: %lu\n", probed);
			(void )fprintf(stderr, "probed count mismatch\n");
			free(map);
			exit(EXIT_FAILURE);
		}

		if (probe.record_id != map[i] || 
		    probe.record_len != bufsiz) {
			prerrors += 1;
			if (debug) {
				(void )fprintf(stderr, 
					       "record: %lu\n"
					       "seq: %lu\n"
					       "rec len: %lu\n"
					       "update_id: %lu\n",
					       probe.record_id,
					       probe.seq_len,
					       probe.record_len,
					       probe.record_update_id);
			}
		}

		err = 0;
		/* check the gibberish */
		for (j = 0; j < bufsiz; j++) {
			if (chkbuf[j] != buf[j]) {
				err = 1;
				rerrors += 1;
			}
		}

		if (debug && err) {
			(void )fprintf(stderr, "expected %s\ngot %s\n",
				       chkbuf, buf);
		}
 
		read += bufsiz;
	}

	free(map);

	(void )fprintf(stderr, 
		       "Read %lu\n"
		       "%lu read errors encountered\n"
		       "%lu probe errors encountered\n", 
		       read, rerrors, prerrors);
	
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
		       "randtest");
	(void )fflush(stderr);
	exit(EXIT_FAILURE);
}

static void
errprnt(const char *s, int errnum)
{

	(void )fprintf(stderr, "randtest %s: %s\n", s, strerror(errnum));
	(void )fflush(stderr);
}
