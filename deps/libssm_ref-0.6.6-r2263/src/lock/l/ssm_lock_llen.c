#include <ssm_lock_ref.h>

size_t ssm_lock_llen(ssm_Il l)
{
  size_t result;
  ssm_lockl lockl = (ssm_lockl) l;

  lockl->lock->share(lockl->lock);
  result = lockl->base->len(lockl->base);
  lockl->lock->rel(lockl->lock);

  return result;
}


