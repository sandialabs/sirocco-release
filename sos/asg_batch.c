#include <stdlib.h>
#include <strings.h>
#include <errno.h>
#include <assert.h>
#include <sys/queue.h>

#include "aasg.h"
#include "aasg_intnl.h"

static void
bsetretv(struct context *ctx) {
	struct BATCH1res *res;
	
	res = &ctx->u.result.b;

	if (ctx->bid->error == -EINPROGRESS) {
		ctx->bid->error = res->status;
		if (ctx->bid->callback != NULL) {
			ctx->bid->callback(ctx->bid->cb_data,
					   ctx->bid->error);
		}
	}
}

/*
  init should establish a pointer to the batch (batch), set up the op cache, 
  and pass back a handle for the client to deal with.
 */

int
asg_batch_init(struct asg_batch_id_t **req,
	       asg_instance_t instance __attribute__ ((unused)),
	       asg_location_t loc,
	       void (*cb)(void *, asg_result_t),
	       void *cb_data) {
	struct asg_batch_id_t *bid;
	int err;

	err = _aasg_bidalloc((struct aasg_peer *)loc, 
			     _aasg_complete, 
			     req);
	if (err)
		return -1;

       	bid = *req;
	bid->len = 0;
	bid->setret = bsetretv;
	bid->callback = cb;
	bid->cb_data = cb_data;
	TAILQ_INIT(&bid->op_queue);

	return 0;
}

int
asg_batch_fini(struct asg_batch_id_t *bid) {
	struct context *ctx;
	size_t i;

	/*
	 * cancel all pending RDMA requests, free all pinned buffers
	 * free all internal memory
	 */

	ctx = bid->ctx;
	for (i = 0; i < ctx->len; i++) {
		(void )_aasg_rdma_cancel_buf(&ctx->rdma[i]);
		if (ctx->rdma[i].pinned && i < 2) {
			free(bid->ctx->rdma[i].iov[0].iov_base);
		}
	}
	free(ctx->rdma);
	free(ctx);

	TAILQ_FOREACH(ctx, &bid->op_queue, _link) {
		for (i = 0; i < ctx->len; i++) {
			(void )_aasg_rdma_cancel_buf(&ctx->rdma[i]);
			if (ctx->rdma[i].pinned == 1) {
				free(ctx->rdma[i].iov[0].iov_base);
			}
		}
		free(ctx->rdma);
	}
		
	while ((ctx = TAILQ_FIRST(&bid->op_queue)) != NULL) {
		TAILQ_REMOVE(&bid->op_queue, ctx, _link);
		free(ctx);
	}

	if (bid->cmdbuf != NULL) {
		free(bid->cmdbuf);
	}
	_aasg_bidfree(bid);
	free(bid);

	return 0;
}

int
asg_batch_return(struct asg_batch_id_t *bid) {
	if (bid == NULL)
		return -1;

	return -aasg_return(bid);
}

int asg_batch_add(struct context *op, struct asg_batch_id_t *bid) {
	TAILQ_INSERT_TAIL(&bid->op_queue, op, _link);
	return 0;
}

int aasg_batch_submit(struct asg_batch_id_t *bid) {
	struct context *ctx;
	int err;
	struct BATCH1args *arg;
	struct context *op;
	ssize_t cc;
	size_t i;
	XDR xdrs;

	ctx = bid->ctx;
	ctx->len = 1;

	ctx->rdma = malloc(sizeof(struct aasg_rdma_buf)*ctx->len);

	if (ctx->rdma == NULL) {
		return -ENOMEM;
	}	

	for (i = 0; i < ctx->len; i++) {
		ctx->rdma[i].pinned = 0;
		ctx->rdma[i].setretv = NULL;
		ctx->rdma[i].ctx = ctx;
		ctx->rdma[i].active = 0;
		ctx->rdma[i].pos = 0;
	}

	do {
		arg = &ctx->u.request.b;
		
		arg->cmdbuf = MBITC(bid->mbitc,
				    (unsigned )bid->id);
		bid->mbitc += 1;

		size_t blen = 0;
		/* calculate size of the command buffer */
		TAILQ_FOREACH(op, &bid->op_queue, _link) {
			blen += sizeof(struct SOSargs);
		}
		
		if (blen > 0) {
			bid->cmdbuf = malloc(blen);
			
			if (bid->cmdbuf == NULL) {
				free(ctx->rdma);
				err = -ENOMEM;
				break;
			}
			memset(bid->cmdbuf, 0, blen);

			xdrmem_create(&xdrs, bid->cmdbuf, blen, XDR_ENCODE);

			TAILQ_FOREACH(op, &bid->op_queue, _link) {
				cc  = xdr_SOSargs(&xdrs, &op->u.request.SOS);

				if (cc == 0) {
					fprintf(stderr, "xdr_SOSargs: %s\n",
						strerror(-cc));
					break;
				}
			}

			ctx->rdma[0].iov[0].iov_base = bid->cmdbuf;
			ctx->rdma[0].iov[0].iov_len = xdr_getpos(&xdrs);

			arg->bufsiz = xdr_getpos(&xdrs);
		
			err = 
				_aasg_rdma_post_buf(bid->aasgp,
						    XM_F_IFRDONLY,
						    arg->cmdbuf,
						    &ctx->rdma[0]);
			if (err) {
				fprintf(stderr, "_aasg_rdma_post_buf: %s\n",
					strerror(-err));
				break;
			}
		} else {
			arg->bufsiz = 0;
		}

		cc = _aasg_pserdes(bid,
				   (xdrproc_t )xdr_BATCH1args, 
				   arg,
				   XDR_ENCODE);

		if (cc < 0) {
			err = (int )cc;
			fprintf(stderr, "_aasg_pserdes: %s\n",
				strerror(-err));
			break;
		}

		err = _aasg_request(bid,
				    SOSBATCH1_OP,
				    &bid->raw, (size_t )cc,
				    &bid->raw, sizeof(bid->raw));
		if (err) {
			fprintf(stderr, "_aasg_request: %s\n",
				strerror(-err));
			break;
		}
		return 0;
	} while (0);

	if (bid->cmdbuf != NULL) 
		free(bid->cmdbuf);

	return -1;
}

int asg_batch_submit(struct asg_batch_id_t *bid) {
	int err;

	if (bid == NULL) {
		return -1;
	}

	err = aasg_batch_submit(bid);

	if (err)
		return err;
	err = -aasg_return(bid);

	asg_batch_fini(bid);

	return err;
}
