#include <ssm_ref.h>

void *ssm_drop_fe_findmr(void *arg, void *buf)
{
  ssm_buf b = buf;
  if(b->mmr.mr == arg)
    return b;
  return NULL;
}

int ssm_drop(ssm_id id, ssm_me me, ssm_mr mr)
{
  if(! id->mes->has(id->mes, me))
    return SSM_REMOVE_INVALID_ME;
  ssm_buf result = ssm_i_foreach(me->bufs->i(me->bufs), ssm_drop_fe_findmr, mr);
  if(me->bufs->rem(me->bufs, result) <= 0)
    return SSM_REMOVE_INVALID_BUF;
  ssm_mr_destroy(result->hdr.mr);
  SSM_DELETE(result);
  return SSM_REMOVE_OK;
}
