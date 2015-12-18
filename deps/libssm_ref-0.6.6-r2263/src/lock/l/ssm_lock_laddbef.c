#include <ssm_lock_ref.h>

int ssm_lock_laddbef(ssm_Il l, void *d, void *a)
{
  int result;
  ssm_lockl lockl = (ssm_lockl) l;

  lockl->lock->arm(lockl->lock);
  result = lockl->base->addbef(lockl->base, d, a);
  lockl->lock->rel(lockl->lock);

  return result;
}
