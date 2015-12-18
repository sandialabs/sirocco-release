#include <ssm_ref.h>

int ssm_update(ssm_id id)
{
  if(id->pendtxs->top(id->pendtxs))
  {
    ssm_tx tx = id->pendtxs->remtop(id->pendtxs);
    id->livetxs->addbot(id->livetxs, tx);
    ssm_Il headl = ssm_ll_l(SSM_NOF);
    ssm_Il mmrl = ssm_ll_l(SSM_NOF);
    headl->addtop(headl, &(tx->buf->hdr));
    mmrl->addtop(mmrl, &(tx->buf->mmr));
    ssm_Il txl = ssm_ll_l(SSM_NOF);
    txl->addtop(txl, tx);
    ssm_Il mdl = ssm_ll_l(SSM_NOF); 
    mdl->addtop(mdl, tx->md);
    if(tx->op == SSM_OP_PUT)
      id->tp->push(id->tp, txl, tx->addr, headl, mmrl, mdl);
    else
      id->tp->pull(id->tp, txl, tx->addr, headl, mmrl, mdl);
    headl->del(headl);
    mmrl->del(mmrl);
    txl->del(txl);
    mdl->del(mdl);
    return 1;
  }
  return 0;
}
