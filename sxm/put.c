#include <errno.h>
#include <assert.h>
#include <stdio.h>

#include "sxm.h"

static void
usage(char *name)
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "        %s <port> <remote_addr> <remote_port>\n",
		name);
	fprintf(stderr, "        %s <port>\n", name);
	return;
}

struct recv {
	struct xm_match_entry *xmme;
	struct xm_message msg;
	int test[1];
	ssm_Haddr addr;
	struct iovec iov;
};

static void
my_xm_cb(struct xm_transport *xmx, struct xm_message *msg)
{
	size_t i;
	printf("xm_transport xmx  = %p\n", xmx);
	printf("xm_message msg    = %p\n", msg);
	printf("unsigned flags    = %u\n", msg->flags);
	printf("ssm_Haddr addr    = %p\n", msg->addr);
	printf("ssm_bits match    = %lx\n", msg->match);
	printf("struct iovec iov  = \n");
	for (i = 0; i < msg->iovcnt; i++) {
		printf("                  %p, %lu\n", msg->iov[i].iov_base,
		       msg->iov[i].iov_len);
	}
	printf("uint64_t len      = %lu\n", msg->len);
	printf("int error         = %i\n", msg->error);
	printf("callback callback = %p\n", msg->callback);
	printf("data.ptr          = %p\n", msg->data.ptr);
	printf("\n");
	if (XM_F_ISRW(msg->flags) == 0) {
		return;
	}

	struct recv *r = msg->data.ptr;
	if (msg->addr != NULL && r != NULL) {
		r->addr = ssm_addr_cp(xmx->ssm, msg->addr);
		if (r->addr == NULL)
			abort();
	} else {
		abort();
	}
}

void
post_recv(struct xm_transport *xmx, struct recv *r)
{
	int rc;
	r->iov.iov_base = r->test;
	r->iov.iov_len = sizeof(int);
	struct xm_message xmmsg = {
		.flags = 0,
		.addr = NULL,
		.match = 0x1234,
		.iov = &r->iov,
		.iovcnt = 1,
		.len = r->iov.iov_len,
		.error = EINPROGRESS,
		.callback = my_xm_cb,
		.data.ptr = r,
	};
	r->msg = xmmsg;
	rc = xm_match_create(xmx, 0x1234, 0, &r->xmme);
	if (rc)
		abort();

	rc = xm_match_insert(r->xmme, XM_MATCH_HEAD, NULL);
	if (rc)
		abort();
	rc = xm_match_add_to_pool(r->xmme, &r->msg);
	if (rc)
		abort();
}

struct send {
	struct xm_message msg;
	int test[1];
	struct iovec iov;
};

static void
do_send(struct xm_transport *xmx, ssm_Haddr addr, struct send *s)
{
	int rc;
	s->iov.iov_base = s->test;
	s->iov.iov_len = sizeof(int);
	struct xm_message xmmsg = {
		.flags = 0,
		.addr = addr,
		.match = 0x1234,
		.iov = &s->iov,
		.iovcnt = 1,
		.len = s->iov.iov_len,
		.error = EINPROGRESS,
		.callback = my_xm_cb,
		.data.ptr = NULL,
	};
	s->msg = xmmsg;
	s->test[0] = 4;
	rc = xm_put(xmx, &s->msg);
	assert(rc == 0);
}

int
main(int argc, char **argv)
{
	if (argc != 2 && argc != 4) {
		usage(argv[0]);
		exit(1);
	}

	uint16_t port = (uint16_t)atoi(argv[1]);
	ssm_Itp itp = ssmptcp_new_tp(port, SSM_NOF);
	struct xm_transport xmx;
	ssm_id ssm = xmx.ssm = ssm_start(itp, NULL, SSM_NOF);

	struct recv r;
	struct send s;
	ssm_Iaddr addrs = ssm_addr(xmx.ssm);
	ssm_Haddr addr = NULL;
	if (argc == 4) {
		/* Post a message */
		post_recv(&xmx, &r);
		/* Send a message */
		ssmptcp_addrargs_t addrargs = {
			.host = argv[2],
			.port = (uint16_t)atoi(argv[3]),
		};
		addr = addrs->create(addrs, &addrargs);
		do_send(&xmx, addr, &s);
		ssm_wait(ssm, NULL);
		/* Wait for reply */
		if (r.msg.error == EINPROGRESS)
			ssm_wait(xmx.ssm, NULL);
		assert(r.test[0] == s.test[0]);
	} else {
		post_recv(&xmx, &r);
		/* wait for a message */
		ssm_wait(xmx.ssm, NULL);
		/* Send a response */
		do_send(&xmx, r.addr, &s);
		ssm_wait(xmx.ssm, NULL);
	}
	/* Clean up */
	xm_match_remove(r.xmme);
	xm_match_destroy(r.xmme);
	ssm_addr_destroy(ssm, r.addr);
	ssm_addr_destroy(ssm, addr);
	ssm_addr_destroy(ssm, addrs);

	ssm_stop(ssm);
	free(itp);
	return 0;
}
