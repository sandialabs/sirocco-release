#include <assert.h>
#include <errno.h>
#include <sys/queue.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <sxm.h>

#include "xmu_addr.h"
#include "sos_prot.h"
#include "aasg_intnl.h"

#define UNEX_MBITS AASG_UXID
#define UNEX_MSIZE AASG_RAWBUFSIZ

xm_match_t data_match;
xm_match_t ret_match;
ssm_Haddr serveraddr;
struct xm_match_entry *ret_me, *data_me;
struct xm_transport *xmx;

size_t wrsize = 64*1024;

int err = EINPROGRESS;
static void
cb(struct xm_transport *xmx, struct xm_message *m)
{
	size_t i;
	printf("xm_transport xmx  = %p\n", xmx);
	printf("xm_message m    = %p\n", m);
	printf("unsigned flags    = %u\n", m->flags);
	printf("ssm_Haddr addr    = %p\n", m->addr);
	printf("ssm_bits match    = %lx\n", m->match);
	printf("struct iovec iov  = \n");
	for (i = 0; i < m->iovcnt; i++) {
		printf("                  %p, %lu\n", m->iov[i].iov_base,
		       m->iov[i].iov_len);
	}
	printf("uint64_t len      = %lu\n", m->len);
	printf("int error         = %i\n", m->error);
	printf("callback callback = %p\n", m->callback);
	printf("data.ptr          = %p\n", m->data.ptr);
	printf("\n");
	printf("cb\n");
	err = m->error;
}

static void
cb_noop(struct xm_transport *xmx, struct xm_message *m)
{
	size_t i;
	printf("xm_transport xmx  = %p\n", xmx);
	printf("xm_message m    = %p\n", m);
	printf("unsigned flags    = %u\n", m->flags);
	printf("ssm_Haddr addr    = %p\n", m->addr);
	printf("ssm_bits match    = %lx\n", m->match);
	printf("struct iovec iov  = \n");
	for (i = 0; i < m->iovcnt; i++) {
		printf("                  %p, %lu\n", m->iov[i].iov_base,
		       m->iov[i].iov_len);
	}
	printf("uint64_t len      = %lu\n", m->len);
	printf("int error         = %i\n", m->error);
	printf("callback callback = %p\n", m->callback);
	printf("data.ptr          = %p\n", m->data.ptr);
	printf("\n");
	printf("cb_noop\n");
	if (m->error != 0)
		err = m->error;
	return;
}

static void
event_loop(void)
{
	int rc;
	size_t xdrlen;
	char *xdrbuf = malloc(UNEX_MSIZE);
	char *buf = malloc(wrsize);
	if (buf == NULL || xdrbuf == NULL) {
		fprintf(stderr, "Could not malloc data or command (%lub): "
			"%s\n", wrsize + UNEX_MSIZE, strerror(errno));
		abort();
	}
	memset(buf, '\0', wrsize);
	struct iovec d_iov = {
		.iov_base = buf,
		.iov_len = wrsize,
	};
	struct WRITE1args wa = {
		.nsid = {
			.container = 0,
			.object = 0,
			.fork = 0,
		},
		.at = 0,
		.nrecs = wrsize,
		.rlen = 1,
		.cond = 0,
		.updctl = 0,
		.update = 1,
		.match = data_match,
	};
	printf("data_match set to %lx\n", data_match);
	struct iovec iov, ret_iov;

	struct xm_message m = {
		.flags = XM_F_IFRW,
		.addr = serveraddr,
		.match = UNEX_MBITS,
		.iov = &iov,
		.iovcnt = 1,
		.callback = cb_noop,
		.data.ptr = NULL,
	};
	struct xm_message data = {
		.flags = XM_F_IFRW,
		.addr = serveraddr,
		.match = wa.match,
		.iov = &d_iov,
		.iovcnt = 1,
		.callback = cb_noop,
		.data.ptr = NULL,
	};
	struct xm_message ret = {
		.flags = XM_F_IFRW,
		.addr = serveraddr,
		.match = ret_match,
		.iov = &ret_iov,
		.iovcnt = 1,
		.callback = cb,
		.data.ptr = NULL,
	};
	printf("Posting ret on %lx\n", ret.match);

	struct msg_header head;
	head.proc = 1;
	head.reply = ret_match;

	XDR xdrs;
	xdrmem_create(&xdrs, xdrbuf, UNEX_MSIZE, XDR_ENCODE);
	assert(msg_header_xdr(&xdrs, &head));
	assert(xdr_WRITE1args(&xdrs, &wa));
	xdrlen = xdr_getpos(&xdrs);

	iov.iov_base = xdrbuf;
	iov.iov_len = xdrlen;
	ret_iov.iov_base = xdrbuf;
	ret_iov.iov_len = UNEX_MSIZE;

	if ((rc = xm_match_add_to_pool(data_me, &data))) {
		fprintf(stderr, "Could not post data match entry: %s\n",
			strerror(rc));
		abort();
	}

	if ((rc = xm_match_add_to_pool(ret_me, &ret))) {
		fprintf(stderr, "Could not post return match entry: %s\n",
			strerror(rc));
		abort();
	}

	xm_put(xmx, &m);
	printf("Beginning wait\n");
	struct timeval time = {
		.tv_sec = 100,
		.tv_usec = 0,
	};
	while(err == EINPROGRESS) {
		printf("Waiting...\n");
		ssm_wait(xmx->ssm, &time);
	}
	printf("Not waiting anymore\n");
}

static void
usage(char *progname)
{
	fprintf(stderr, "Usage: %s <listen_port> <server_ip>\n",
		progname);
}

int
main(int argc, char **argv)
{
	int rc;
	long lport;
	char *ctmp;

	if (argc != 3) {
		usage(argv[0]);
		exit(1);
	}

	lport = strtol(argv[1], &ctmp, 10);
	if (lport < 0 || lport > UINT16_MAX || *ctmp != '\0') {
		fprintf(stderr, "Invalid value for port %s\n", argv[1]);
		usage(argv[0]);
		exit(1);
	}

	rc = xmtcp_start(&xmx, lport);
	if (rc != 0) {
		printf("Could not start SXM: %s\n", strerror(-rc));
		abort();
	}

	rc = xmu_parse_addr(argv[2], xmx, &serveraddr);
	if (rc) {
		fprintf(stderr, "Could not parse address %s: %s\n",
			argv[2], strerror(-rc));
		abort();
	}

	srand(time(NULL));
	data_match = rand();
	ret_match = rand();

	printf("Posting data match on %lx, return match on %lx\n",
	       data_match, ret_match);
	if ((rc = xm_match_create(xmx, data_match, 0, &data_me))) {
		printf("Could not create data match entry: %s\n",
		       strerror(-rc));
		abort();
	}
	if ((rc = xm_match_create(xmx, ret_match, 0, &ret_me))) {
		printf("Could not create return match entry: %s\n",
		       strerror(-rc));
		abort();
	}

	if ((rc = xm_match_insert(data_me, XM_MATCH_HEAD, NULL))) {
		printf("Could not insert on data match entry: %s\n",
		       strerror(-rc));
		abort();
	}
	if ((rc = xm_match_insert(ret_me, XM_MATCH_HEAD, NULL))) {
		printf("Could not insert on return match entry: %s\n",
		       strerror(-rc));
		abort();
	}

	do {
		event_loop();
		if (err != 0) {
			fprintf(stderr, "Encountered error %i: %s\n", err,
				strerror(err));
		}
	} while (err == ENOBUFS);

	return 0;
}
