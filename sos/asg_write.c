#include <stdlib.h>
#include <strings.h>
#include <errno.h>
#include <assert.h>

#include "aasg.h"
#include "aasg_intnl.h"

static void 
wsetretv(struct context *ctx) {
	WRITE1res *res;
	WRITE1ret *ret;
	bool_t cc;
	XDR xdrs;

	res = &ctx->u.result.SOS.SOSres_u.w;
	ret = &ctx->retv.WRITE1;

	xdrmem_create(&xdrs, ctx->rdma[1].iov[0].iov_base,
		      sizeof(WRITE1res), XDR_DECODE);

	cc = xdr_WRITE1res(&xdrs, res);

	if (cc) {
		*ret->transferred = res->count;
		*ret->new_update_id = res->vers;
	} else {
		*ret->transferred = 0;
		*ret->new_update_id = 0;
	}	
}

static int
master_aasg_write(struct asg_session ses,
		  struct asg_id id,
		  asg_size_t recordlen,
		  asg_flags_t flags,
		  asg_update_id_t update_id_condition,
		  asg_update_id_t *new_update_id,
		  const void *data,
		  asg_size_t *transferred)
{
	cond1 cond;
	unsigned nf;
	update1	upd;
	unsigned nupd;
	int err;
	struct context *ctx;
	WRITE1args *args;
	struct WRITE1ret *retv;
	size_t i;
	struct asg_batch_id_t *bid;

	bid = ses.batch;

	if (bid == NULL) {
		return -EINVAL;
	}

	if ((nf = _aasg_setflags(flags, &cond)) != 0) {
		return nf;
	}

	upd = SOS1_UPD_NONE;
	nupd = 0;
	if (flags & ASG_AUTO_UPDATE_ID) {
		upd = SOS1_UPD_AUTO;
		nupd++;
	} else {
		upd = update_id_condition;
	}
	if (nupd > 1) {
		return -EINVAL;
	}

	ctx = malloc(sizeof(struct context));

	if (ctx == NULL) {
		return -ENOMEM;
	}

	ctx->len = 2;
	ctx->rdma = malloc(sizeof(struct aasg_rdma_buf)*ctx->len);
	
	if (ctx->rdma == NULL) {
		free(ctx);
		return -ENOMEM;
	}

	for (i = 0; i < ctx->len; i++) {
		ctx->rdma[i].pinned = 0;
		ctx->rdma[i].active = 0;
		ctx->rdma[i].pos = 0;
		ctx->rdma[i].ctx = NULL;
		ctx->rdma[i].setretv = NULL;
	}

	ctx->rdma[1].iov[0].iov_base = malloc(sizeof(WRITE1res));
	ctx->rdma[1].iov[0].iov_len = sizeof(WRITE1res);
	ctx->rdma[1].pinned = 1;
	ctx->rdma[1].setretv = wsetretv;
	ctx->rdma[1].ctx = ctx;

	if (ctx->rdma[1].iov[0].iov_base == NULL) {
		free(ctx->rdma);
		free(ctx);
		return -ENOMEM;
	}

	memset(ctx->rdma[1].iov[0].iov_base, 0, sizeof(WRITE1res));
	
	do {
		retv = &ctx->retv.WRITE1;
		retv->new_update_id = new_update_id;
		retv->transferred = transferred;

		args = &ctx->u.request.SOS.SOSargs_u.w;
		_aasg_set_nsid1(&args->nsid, id.c, id.o, id.f);

		ctx->u.request.SOS.op = SOS1_WRITE;
		args->at = id.r;
		args->nrecs = id.rcount;
		args->rlen = recordlen;
		args->cond = cond;
		args->updctl = update_id_condition;
		args->update = upd;
		args->match =
			MBITC(bid->mbitc, (unsigned )bid->id);
		bid->mbitc += 1;
		args->ret = 
			MBITC(bid->mbitc, (unsigned )bid->id);
		bid->mbitc += 1;
		ctx->rdma[0].iov[0].iov_base = (void *)data;
		ctx->rdma[0].iov[0].iov_len = id.rcount * recordlen;
		ctx->rdma[0].pinned = 0;

		err = _aasg_rdma_post_buf(bid->aasgp,
					  XM_F_IFRDONLY,
					  args->match,
					  &ctx->rdma[0]);
		if (err)
			break;
		err = _aasg_rdma_post_buf(bid->aasgp,
					  XM_F_IFWRONLY,
					  args->ret,
					  &ctx->rdma[1]);
		if (err)
			break;

		TAILQ_INSERT_TAIL(&bid->op_queue, ctx, _link);

		return 0;
	} while (0);
	for (i = 0; i < ctx->len; i++) {
		_aasg_rdma_cancel(bid->aasgp, &ctx->rdma[i].message, 0);
		if (ctx->rdma[i].pinned) {
			free(ctx->rdma[i].iov[0].iov_base);
		}
	}
	free(ctx->rdma);
	free(ctx);
	return err;
}

int aasg_write (
      struct asg_session ses,
      struct asg_id id,
      asg_size_t recordlen,
      asg_flags_t flags,
      asg_update_id_t update_id_condition,
      asg_update_id_t *new_update_id,
      const void *data,
      asg_size_t *transferred)
{
	return master_aasg_write(ses, id, recordlen, flags,
				 update_id_condition, new_update_id,
				 data, transferred);
}

int
asg_write(struct asg_session ses,
	  struct asg_id id,
	  asg_size_t recordlen,
	  asg_flags_t flags,
	  asg_update_id_t update_id_condition,
	  asg_update_id_t *new_update_id,
	  const void *data,
	  asg_size_t *transferred)
{
	int err;
	struct asg_batch_id_t *batch;
	err = asg_batch_init(&batch, ses.inst, ses.loc, 
			     NULL, NULL);

	if (err) {
		return err;
	}

	ses.batch = batch;

	err =
		master_aasg_write(ses,
				  id,
				  recordlen,
				  flags,
				  update_id_condition,
				  new_update_id,
				  data,
				  transferred);
	if (err) {
		return err;
	}
	return asg_batch_submit(batch);
}
