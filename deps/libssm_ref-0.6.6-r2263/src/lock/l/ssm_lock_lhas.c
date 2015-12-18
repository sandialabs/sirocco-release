#include <ssm_lock_ref.h>

int ssm_lock_lhas(ssm_Il l, void *data)
{
  int result;
  ssm_lockl lockl = (ssm_lockl) l;

  lockl->lock->arm(lockl->lock);
  result = lockl->base->has(lockl->base, data);
  lockl->lock->rel(lockl->lock);

  return result;
}



