#include <stdlib.h>
#include <strings.h>
#include <errno.h>
#include <assert.h>

#include "aasg.h"
#include "aasg_intnl.h"

static void 
rsetretv(struct context *ctx) {
	READ1res *res;
	READ1ret *ret;
	bool_t cc;
	XDR xdrs;

	res = &ctx->u.result.SOS.SOSres_u.r;
	ret = &ctx->retv.READ1;
	
	xdrmem_create(&xdrs, ctx->rdma[0].iov[0].iov_base,
		      sizeof(READ1res), XDR_DECODE);

	cc = xdr_READ1res(&xdrs, res);

	if (cc) {
		/* determine call type - read, probe or both */
		if (ret->transferred != NULL) {
			*ret->transferred = res->count;
			*ret->update_id_info = res->vers;
		}
		if (ret->record_buf_transferred != NULL) {
			*ret->record_buf_transferred = res->rcount;
		}
	} else {
		if (ret->transferred != NULL) {
			*ret->transferred = 0;
			*ret->update_id_info = 0;
		}
		if (ret->record_buf_transferred != NULL) {
			*ret->record_buf_transferred = 0;
		}
	}
}

static void
prsetretv(struct context *ctx) {
	READ1ret *ret;
	rinfo1 res;
	bool_t cc;
	size_t i;
	XDR xdrs;

	ret = &ctx->retv.READ1;

	i = 1;
	if (ret->bufsiz > 0) {
		i = 2;
	}

	xdrmem_create(&xdrs, ctx->rdma[i].iov[0].iov_base,
		      ret->rbufsiz, XDR_DECODE);

	i = 0;
	while ((cc = xdr_rinfo1(&xdrs, &res)) > 0) {
		ret->record_buf[i].record_id = res.rid;
		ret->record_buf[i].seq_len = res.runlen;
		ret->record_buf[i].record_len = res.rlen;
		ret->record_buf[i].record_update_id = res.vers;
		i++;
	}
}

static int
master_aasg_read(struct asg_session ses,
		 struct asg_id id,
		 asg_flags_t flags,
		 asg_update_id_t update_id_condition,
		 void *buf,
		 size_t bufsize,
		 asg_size_t *transferred,
		 asg_record_info_t *record_buf,
		 asg_size_t rbufsize,
		 asg_size_t *record_buf_transferred,
		 asg_update_id_t *update_id_info)
{
	cond1	cond;
	unsigned nf;
	int	err;
	struct context *ctx;
	READ1args *args;
	struct READ1ret *retv;
	size_t i, rbuf, rinfo;
	struct asg_batch_id_t *bid;

	bid = ses.batch;
	rbuf = 0;
	rinfo = 0;

	if (bid == NULL) {
		return -EINVAL;
	}

	if ((nf = _aasg_setflags(flags, &cond)) != 0) {
		return nf;
	}

	ctx = malloc(sizeof(struct context));

	if (ctx == NULL) {
		return -ENOMEM;
	}

	if (rbufsize > 0 && bufsize > 0) {
		ctx->len = 3;
		rbuf = 1;
		rinfo = 2;
	} else if (rbufsize == 0 && bufsize > 0) {
		ctx->len = 2;
		rbuf = 1;
	} else if (rbufsize > 0 && bufsize == 0) {
		ctx->len = 2;
		rinfo = 1;
	} else {
		free(ctx);
		return -EINVAL;
	}

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

	ctx->rdma[0].iov[0].iov_base = malloc(sizeof(READ1res));
	ctx->rdma[0].iov[0].iov_len = sizeof(READ1res);
	ctx->rdma[0].pinned = 1;
	ctx->rdma[0].setretv = rsetretv;
	ctx->rdma[0].ctx = ctx;

	if (ctx->rdma[0].iov[0].iov_base == NULL) {
		free(ctx->rdma);
		free(ctx);
		return -ENOMEM;
	}

	memset(ctx->rdma[0].iov[0].iov_base, 0, sizeof(READ1res));

	if (rinfo > 0) {
		ctx->rdma[rinfo].iov[0].iov_base = malloc(rbufsize);
		ctx->rdma[rinfo].iov[0].iov_len = rbufsize;
		ctx->rdma[rinfo].pinned = 1;
		ctx->rdma[rinfo].ctx = ctx;
		ctx->rdma[rinfo].setretv = prsetretv;

		if (ctx->rdma[rinfo].iov[0].iov_base == NULL) {
			free(ctx->rdma[0].iov[0].iov_base);
			free(ctx->rdma);
			free(ctx);
			return -ENOMEM;
		}
		memset(ctx->rdma[rinfo].iov[0].iov_base, 0, rbufsize);
	}

	if (rbuf > 0) {
		ctx->rdma[rbuf].iov[0].iov_base = buf;
		ctx->rdma[rbuf].iov[0].iov_len = bufsize;
		ctx->rdma[rbuf].pinned = 0;
		ctx->rdma[rbuf].ctx = ctx;
		ctx->rdma[rbuf].setretv = NULL;
	}

	do {
		retv = &ctx->retv.READ1;
		retv->transferred = transferred;
		retv->record_buf_transferred = record_buf_transferred;
		retv->update_id_info = update_id_info;
		retv->record_buf = record_buf;
		retv->rbufsiz = rbufsize;
		retv->bufsiz = bufsize;
		args = &ctx->u.request.SOS.SOSargs_u.r;
		_aasg_set_nsid1(&args->nsid, id.c, id.o, id.f);
		ctx->u.request.SOS.op = SOS1_READ;
		args->at = id.r;
		args->nrecs = id.rcount;
		args->cond = cond;
		args->update = update_id_condition;
		args->bufsiz = bufsize;
		args->probe.present = 0;
		if (rinfo > 0) {
			args->probe.present = 1;
			args->probe.OPTPROBE1args_u.info.bufsiz = rbufsize;
		}

		args->match = 0;
		args->probe.OPTPROBE1args_u.info.match = 0;

		if (rbuf > 0) {
			args->match =
				MBITC(bid->mbitc, (unsigned )bid->id);
			bid->mbitc += 1;
			err = _aasg_rdma_post_buf(bid->aasgp,
						  XM_F_IFWRONLY,
						  args->match,
						  &ctx->rdma[rbuf]);
			if (err)
				break;

		} if (rinfo > 0) {
			args->probe.present = 1;
			args->probe.OPTPROBE1args_u.info.match = 
				MBITC(bid->mbitc, (unsigned )bid->id);
			bid->mbitc += 1;
			err = _aasg_rdma_post_buf(bid->aasgp,
						  XM_F_IFWRONLY,
						  args->probe.
						  OPTPROBE1args_u.info.match,
						  &ctx->rdma[rinfo]);
			if (err)
				break;
		}

		args->ret = 
			MBITC(bid->mbitc, (unsigned )bid->id);
		bid->mbitc += 1;

		err = _aasg_rdma_post_buf(bid->aasgp,
					  XM_F_IFWRONLY,
					  args->ret,
					  &ctx->rdma[0]);
		
		if (err)
			break;

		TAILQ_INSERT_TAIL(&bid->op_queue, ctx, _link);

		return 0;
	} while (0);
	for (i = 0; i < ctx->len; i++) {
		(void )_aasg_rdma_cancel_buf(&ctx->rdma[i]);
		if (ctx->rdma[i].pinned == TRUE)
			free(ctx->rdma[i].iov[0].iov_base);
	}
	free(ctx->rdma);
	free(ctx);
	return err;
}

int
aasg_read(struct asg_session ses,
	  struct asg_id id,
	  asg_flags_t flags,
	  asg_update_id_t update_id_condition,
	  void *buf,
	  asg_size_t bufsize,
	  asg_size_t *transferred,
	  asg_record_info_t *record_buf,
	  asg_size_t rbufsize,
	  asg_size_t *record_buf_transferred,
	  asg_update_id_t *update_id_info) {
	return master_aasg_read(ses, id, flags, update_id_condition,
				buf, bufsize, transferred, record_buf,
				rbufsize, record_buf_transferred,
				update_id_info);
}

int
asg_read(struct asg_session ses,
	 struct asg_id id,
	 asg_flags_t flags,
	 asg_update_id_t update_id_condition,
	 void *buf,
	 size_t bufsize,
	 asg_size_t *transferred,
	 asg_record_info_t *record_buf,
	 asg_size_t rbufsize,
	 asg_size_t *record_buf_transferred,
	 asg_update_id_t *update_id_info)
{
	int	err;
	struct asg_batch_id_t *batch;

	err = asg_batch_init(&batch, ses.inst, ses.loc,
			     NULL, NULL);

	if (err)
		return err;

	ses.batch = batch;

	err =
	    master_aasg_read(ses,
			     id,
			     flags,
			     update_id_condition,
			     buf, 
			     bufsize,
			     transferred,
			     record_buf, 
			     rbufsize,
			     record_buf_transferred,
			     update_id_info);

	if (err)
		return err;
	return asg_batch_submit(batch);
}
