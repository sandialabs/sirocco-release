#include <stdlib.h>
#include <strings.h>
#include <errno.h>
#include <assert.h>

#include "aasg.h"
#include "aasg_intnl.h"

static void
reply_handler(struct xm_transport *xmx __attribute__ ((unused)),
	      struct xm_message *xmmsg)
{
	struct asg_batch_id_t *bid;

	bid = xmmsg->data.ptr;
	assert(xmmsg == &bid->msg[0]);
	

	if (bid->completion_handler != NULL) {
		bid->completion_handler(bid);
	}
	//_aasg_rdma_cancel(bid->aasgp, xmmsg);
}

static void
send_handler(struct xm_transport *xmx, struct xm_message *xmmsg)
{
	struct asg_batch_id_t *bid;

	bid = xmmsg->data.ptr;
	assert(xmmsg == &bid->msg[1]);
	if (xmmsg->error) {
		xmmsg = &bid->msg[0];
		_aasg_rdma_cancel(bid->aasgp, xmmsg, 0);
		xmmsg->error = 0;
		reply_handler(xmx, xmmsg);
	}
}

static bool_t
xdr_aasg_request_header(XDR *xdrs, struct aasg_request_header *objp)
{

	if (!xdr_uint32_t(xdrs, &objp->proc))
		return FALSE;
	if (!xdr_uint64_t(xdrs, &objp->reply))
		return FALSE;
	return TRUE;
}

int
_aasg_request(struct asg_batch_id_t *aasgctx,
	      uint32_t proc,
	      void *args, size_t sizargs,
	      void *res, size_t sizres)
{
	int	err;
	struct xm_message *xmmsg;

	aasgctx->header.proc = proc;
	aasgctx->header.reply = MBITC(AASG_GRPID_REPLY, 
				      aasgctx->id);
	(void )_aasg_serdes((xdrproc_t )xdr_aasg_request_header,
			    &aasgctx->header,
			    &aasgctx->raw_header, sizeof(aasgctx->raw_header),
			    XDR_ENCODE);
	do {
		if (!sizres)
			break;
		aasgctx->iov[0].iov_base = res;
		aasgctx->iov[0].iov_len = sizres;
		err =
		    _aasg_rdma_post(aasgctx->aasgp, 
				    XM_F_IFWRONLY,
				    aasgctx->header.reply,
				    &aasgctx->iov[0],
				    1,
				    reply_handler,
				    aasgctx,
				    &aasgctx->msg[0],
				    0);
		if (err)
			return err;
	} while (0);
	aasgctx->iov[1].iov_base = aasgctx->raw_header;
	aasgctx->iov[1].iov_len = sizeof(aasgctx->raw_header);
	aasgctx->iov[2].iov_base = args;
	aasgctx->iov[2].iov_len = sizargs;
	xmmsg = &aasgctx->msg[1];
	(void )memset(xmmsg, 0, sizeof(struct xm_message));
	xmmsg->flags = XM_F_IFRDONLY;
	xmmsg->addr = aasgctx->aasgp->addr;
	xmmsg->match = AASG_UXID;
	xmmsg->iov = aasgctx->iov + 1;
	xmmsg->iovcnt = 2;
	xmmsg->callback = send_handler;
	xmmsg->data.ptr = aasgctx;
	do {
		err = xm_send(aasgctx->aasgp->aasgx->xmx, xmmsg);
		if (err)
			break;
		return 0;
	} while (0);
	_aasg_rdma_cancel(aasgctx->aasgp, xmmsg, 0);
	return err;
}
