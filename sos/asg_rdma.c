#include <stdlib.h>
#include <strings.h>
#include <errno.h>
#include <assert.h>

#include "aasg.h"
#include "aasg_intnl.h"

int
_aasg_rdma_post(struct aasg_peer *aasgp,
		unsigned flags,
		xm_match_t match,
		const struct iovec *iov,
		size_t iovcnt,
		void (*callback)(struct xm_transport *, struct xm_message *),
		void *data,
		struct xm_message *xmmsg,
		size_t pos)
{
	unsigned mapid;
	int	nr;

	(void )memset(xmmsg, 0, sizeof(struct xm_message));
	xmmsg->flags = flags;
	xmmsg->addr = aasgp->addr;
	xmmsg->match = match;
	xmmsg->iov = iov;
	xmmsg->iovcnt = iovcnt;
	xmmsg->callback = callback;
	xmmsg->data.ptr = data;
	mapid = MBITC_MAP(match);
	assert(mapid < AASG_NGRP);
	nr = MBITC_NR(match);
	assert((unsigned )nr < AASG_CALLS_MAX);
	assert((unsigned )pos < AASG_DEPTH);

	return xm_match_add_to_pool(aasgp->aasgx->grp[mapid][nr][pos], xmmsg);
}

int
_aasg_rdma_cancel(struct aasg_peer *aasgp, struct xm_message *xmmsg, size_t pos)
{
	struct xm_match_entry *xmme;
	unsigned mapid;
	int nr;

	mapid = MBITC_MAP(xmmsg->match);
	nr = MBITC_NR(xmmsg->match);
	assert((unsigned )pos < AASG_DEPTH);

	xmme =
	    aasgp->aasgx->grp[mapid][nr][pos];
	#if NDEBUG
	fprintf(stderr, "removing %p\n", xmmsg);
	#endif

	return xm_match_remove_from_pool(xmme, xmmsg);
}

static void
rdma_handler(struct xm_transport *xmx __attribute__ ((unused)),
	     struct xm_message *xmmsg)
{
	struct aasg_rdma_buf *aasgrb;
#if NDEBUG
	int err;
#endif

	aasgrb = xmmsg->data.ptr;
	if (!aasgrb->active)
		return;
	if (aasgrb->pinned) {
		if (aasgrb->setretv != NULL) {
			aasgrb->setretv(aasgrb->ctx);
		} 
	}
#if NDEBUG
	err = 
#else 
	(void )
#endif 
	_aasg_rdma_cancel(aasgrb->aasgp, xmmsg, aasgrb->pos);
	#if NDEBUG
	if (err)
		fprintf(stderr, "rdma_handler: %s\n", strerror(-err));
	#endif
	aasgrb->active = 0;
}

int
_aasg_rdma_post_buf(struct aasg_peer *aasgp,
		    unsigned flags,
		    xm_match_t match,
		    struct aasg_rdma_buf *aasgrb)
{
	int	err;

	aasgrb->active = 0;
	aasgrb->aasgp = aasgp;
	err =
	    _aasg_rdma_post(aasgp,
			    flags,
			    match,
			    (const struct iovec *)aasgrb->iov,
			    sizeof(aasgrb->iov) / sizeof(struct iovec),
			    rdma_handler,
			    aasgrb,
			    &aasgrb->message,
			    aasgrb->pos);
	if (err) {
		fprintf(stderr, "_aasg_rdma_post_buf: %s\n",
			strerror(-err));
		return err;
	}
	aasgrb->active = 1;

	#if NDEBUG
	fprintf(stderr, "POSTED BUF with m 0x%lx\n", 
		match);
	#endif

	return 0;
}

int
_aasg_rdma_cancel_buf(struct aasg_rdma_buf *aasgrb)
{
	int err;

	#if NDEBUG
	fprintf(stderr, "CANCEL BUF with m 0x%lx\n", aasgrb->message.match);
	#endif

	if (!aasgrb->active) {
		return -EINVAL;
	}
	err = _aasg_rdma_cancel(aasgrb->aasgp, &aasgrb->message, aasgrb->pos);
	#if NDEBUG
	assert(!err);
	#endif 
	if (err) {
		fprintf(stderr, "_aasg_rdma_cancel_buf: %s\n", 
			strerror(-err));
		return err;
	}
	aasgrb->active = 0;
	return err;
}

/*
each buffer is either pinned in sos's memory (aasg_rdma_buf.pinned) or not
Each pinned buffer should be freed upon completion of its job (when the 
result is written into the buffer and then unserialized.

So, when the callback fires for each pinned buffer, the setretv should fire.
*/
