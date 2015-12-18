#include <ssm_lock_ref.h>

void *ssm_lock_lbot(ssm_Il l)
{
  void * result;
  ssm_lockl lockl = (ssm_lockl) l;

  lockl->lock->share(lockl->lock);
  result = lockl->base->bot(lockl->base);
  lockl->lock->rel(lockl->lock);

  return result;
}


