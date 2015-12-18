#include <ssm_lock_ref.h>

int ssm_lock_laddbot(ssm_Il l, void *d)
{
  int result;
  ssm_lockl lockl = (ssm_lockl) l;

  lockl->lock->arm(lockl->lock);
  result = lockl->base->addbot(lockl->base, d);
  lockl->lock->rel(lockl->lock);

  return result;
}

