#include <ssm_lock_ref.h>

int ssm_lock_ldel(ssm_Il l)
{
  int result;
  ssm_lockl lockl = (ssm_lockl) l;

  lockl->lock->arm(lockl->lock);
  result = lockl->base->del(lockl->base);
  lockl->lock->rel(lockl->lock);

  lockl->lock->del(lockl->lock);
  SSM_DELETE(l);

  return result;
}




