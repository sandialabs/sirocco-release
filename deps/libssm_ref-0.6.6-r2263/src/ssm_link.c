#include <ssm_ref.h>

ssm_me ssm_link(ssm_id id, ssm_bits bits, ssm_bits mask, ssm_pos pos, ssm_me anchor, ssm_cb cb, ssm_Flink flags)
{
  ssm_me me = SSM_NEW(me);
  me->owner = id;
  me->bufs = ssm_lock_l(ssm_spinlock_lock(), ssm_ll_l(SSM_NOF));
  me->bits = bits;
  me->mask = mask;
  me->cb = cb;
  me->flags = flags;

  switch(pos)
  {
    case SSM_POS_HEAD:
      id->mes->addtop(id->mes, me);
      break;
    case SSM_POS_TAIL:
      id->mes->addbot(id->mes, me);
      break;
    case SSM_POS_BEFORE:
      id->mes->addbef(id->mes, me, anchor);
      break;
    case SSM_POS_AFTER:
      id->mes->addaft(id->mes, me, anchor);
      break;
  }
  return me;
}
