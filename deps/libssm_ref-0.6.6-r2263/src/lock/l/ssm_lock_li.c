#include <ssm_lock_ref.h>

ssm_Ii ssm_lock_li(ssm_Il l)
{
  ssm_dtrace();
  ssm_lockl lockl = (ssm_lockl) l;

  lockl->lock->share(lockl->lock);
  return ssm_lock_i(lockl->lock, lockl->base->i(lockl->base));
}


