#include <ssm_ref.h>

extern int ssm_cancel(ssm_id id, ssm_tx tx)
{
  if(id->pendtxs->rem(id->pendtxs, tx))
  {
    ssm_result result = SSM_NEW(result);
    result->id = id;
    result->me = tx->me;
    result->op = tx->op;
    result->md = tx->md;
    result->bits = tx->bits;
    result->mr = tx->buf->mmr.mr;
    result->bytes = 0;
    result->addr = tx->addr;
    result->tx = tx;
    result->status = SSM_ST_CANCEL;
    id->evs->addbot(id->evs, ssm_cb_ev(tx->cb, result));
  }
  return 0;
}
