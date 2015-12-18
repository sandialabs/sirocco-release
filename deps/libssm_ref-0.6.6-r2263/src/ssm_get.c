#include <ssm_ref.h>

ssm_tx ssm_get(ssm_id id, ssm_Haddr addr, ssm_md target, ssm_mr src, ssm_bits bits, ssm_cb cb, ssm_Fop flags)
{
  ssm_passert(id, addr, src, cb);
  
  ssm_tx tx = SSM_NEW(tx);
  tx->buf = SSM_NEW(buf);
  tx->owner = id;
  tx->cb = cb;
  tx->md = target;
  tx->me = NULL;
  tx->op = SSM_OP_GET;
  tx->bits = bits;
  tx->txflags = flags;
  tx->addr = addr;
  tx->cancel = 0;
  tx->buf->mmr.mr = src;
  tx->buf->mmr.flags = SSM_NOF;
  tx->buf->hdr.mr = ssm_hdr_mr(bits, SSM_OP_GET, src);
  tx->buf->hdr.flags = SSM_MAP_HEAD;

  ssm_Il l = ssm_ll_l(SSM_NOF);
  l->addtop(l, &(tx->buf->mmr));
  l->addtop(l, &(tx->buf->hdr));
  id->tp->map(id->tp, l);
  l->del(l);
  
  id->pendtxs->addtop(id->pendtxs, tx);
  id->tp->wake(id->tp);
  return tx;
}

