#include <ssm_lock_ref.h>

int ssm_lock_aadel(ssm_Iaa aa)
{
  int result;
  ssm_lockaa lockaa = (ssm_lockaa) aa;

  lockaa->lock->arm(lockaa->lock);
  result = lockaa->base->del(lockaa->base);
  lockaa->lock->rel(lockaa->lock);

  lockaa->lock->del(lockaa->lock);
  
  return result;
}
