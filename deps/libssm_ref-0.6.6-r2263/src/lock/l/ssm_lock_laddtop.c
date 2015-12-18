#include <ssm_lock_ref.h>

int ssm_lock_laddtop(ssm_Il l, void *d)
{
  int result;
  ssm_lockl lockl = (ssm_lockl) l;

  lockl->lock->arm(lockl->lock);
  result = lockl->base->addtop(lockl->base, d);
  lockl->lock->rel(lockl->lock);

  return result;
}


