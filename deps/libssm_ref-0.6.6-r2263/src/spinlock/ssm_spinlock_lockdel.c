#include <ssm_spinlock_ref.h>

int ssm_spinlock_lockdel(ssm_Ilock lock)
{
  ssm_spinlocklock sll = (ssm_spinlocklock)lock;
  pthread_spin_destroy(&(sll->psl));
  SSM_DELETE(sll);
  return 0;
}
