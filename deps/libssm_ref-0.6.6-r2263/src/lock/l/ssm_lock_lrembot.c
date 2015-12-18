#include <ssm_lock_ref.h>

void * ssm_lock_lrembot(ssm_Il l)
{
  void * result;
  ssm_lockl lockl = (ssm_lockl) l;

  lockl->lock->arm(lockl->lock);
  result = lockl->base->rembot(lockl->base);
  lockl->lock->rel(lockl->lock);

  return result;
}





