#include <ssm_ref.h>

void * ssm_unlink_foreach_freebuf(void *arg, void *buf)
{
  SSM_DEL(buf);
  return NULL;
}

int ssm_unlink(ssm_id id, ssm_me me)
{
  if(! id->mes->has(id->mes, me))
    return SSM_REMOVE_INVALID;

  if(me->bufs->len(me->bufs) > 0)
    return SSM_REMOVE_BUSY;

  id->mes->rem(id->mes, me);
  me->bufs->del(me->bufs);
  SSM_DEL(me);
  return 0;
}
