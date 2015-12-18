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

ssm_Haddr addr;
struct xm_match_entry *xmme;
struct xm_message send_msg, recv_msg;
int data[1024];
struct iovec iovs[] = {
	{
		.iov_base = data,
		.iov_len = sizeof(int) * 128,
	},
	{
		.iov_base = data + 128,
		.iov_len = sizeof(int) * 512,
	},
	{
		.iov_base = data + 640,
		.iov_len = sizeof(int) * 256,
	},
	{
		.iov_base = data + 896,
		.iov_len = sizeof(int) * 128,
	},
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
	printf("struct iovec iov[%lu]  = \n", msg->iovcnt);
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

	if (msg->addr != NULL) {
		addr = ssm_addr_cp(xmx->ssm, msg->addr);
		printf("Setting reply as %p\n", addr);
		if (addr == NULL)
			abort();
	} else {
		printf("Return address is null, aborting\n");
		abort();
	}
}

void
post_recv(struct xm_transport *xmx)
{
	int rc;
	struct xm_message xmmsg = {
		.flags = 0,
		.addr = NULL,
		.match = 0x1234,
		.iov = iovs,
		.iovcnt = sizeof(iovs)/sizeof(iovs[0]),
		.len = 0,
		.error = EINPROGRESS,
		.callback = my_xm_cb,
		.data.ptr = NULL,
	};
	recv_msg = xmmsg;
	rc = xm_match_create(xmx, 0x1234, 0, &xmme);
	if (rc)
		abort();

	rc = xm_match_insert(xmme, XM_MATCH_HEAD, NULL);
	if (rc)
		abort();
	rc = xm_match_add_to_pool(xmme, &recv_msg);
	if (rc)
		abort();
}

static void
do_send(struct xm_transport *xmx, ssm_Haddr addr)
{
	int rc;
	struct xm_message xmmsg = {
		.flags = 0,
		.addr = addr,
		.match = 0x1234,
		.iov = iovs,
		.iovcnt = sizeof(iovs)/sizeof(iovs[0]),
		.len = 0,
		.error = EINPROGRESS,
		.callback = my_xm_cb,
		.data.ptr = NULL,
	};
	send_msg = xmmsg;
	rc = xm_put(xmx, &send_msg);
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

	ssm_Iaddr addrs = ssm_addr(xmx.ssm);
	size_t i;

	if (argc == 4) {
		/* Post a message */
		post_recv(&xmx);
		/* Send a message */
		ssmptcp_addrargs_t addrargs = {
			.host = argv[2],
			.port = (uint16_t)atoi(argv[3]),
		};
		addr = addrs->create(addrs, &addrargs);

		for (i = 0; i < 1024; i++)
			data[i] = i;

		do_send(&xmx, addr);
		ssm_wait(ssm, NULL);
		/* Wait for reply */
		if (recv_msg.error == EINPROGRESS)
			ssm_wait(xmx.ssm, NULL);
	} else {
		post_recv(&xmx);
		/* wait for a message */
		ssm_wait(xmx.ssm, NULL);

		for (i = 0; i < 1024; i++)
			assert(data[i] == (int)i);

		printf("Replying to %p\n", addr);
		/* Send a response */
		do_send(&xmx, addr);
		ssm_wait(xmx.ssm, NULL);
	}
	printf("Done with messaging\n");
	/* Clean up */
	xm_match_remove(xmme);
	xm_match_destroy(xmme);
	ssm_addr_destroy(ssm, addr);
	ssm_addr_destroy(ssm, addrs);

	ssm_stop(ssm);
	free(itp);
	return 0;
}
