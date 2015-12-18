#include <ssm_spinlock_ref.h>

ssm_Ilock ssm_spinlock_lock()
{
  ssm_spinlocklock sll = SSM_NEW(spinlocklock);
  sll->lock.arm = ssm_spinlock_lockarm;
  sll->lock.share = ssm_spinlock_lockshare;
  sll->lock.rel = ssm_spinlock_lockrel;
  sll->lock.del = ssm_spinlock_lockdel;
  pthread_spin_init(&(sll->psl), 0);
  return (ssm_Ilock)sll;
}
