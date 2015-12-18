#include <ssm_spinlock_ref.h>

int ssm_spinlock_lockarm(ssm_Ilock lock)
{
  ssm_spinlocklock sll = (ssm_spinlocklock) lock;
  pthread_spin_lock(&(sll->psl));
  return 0;
}
