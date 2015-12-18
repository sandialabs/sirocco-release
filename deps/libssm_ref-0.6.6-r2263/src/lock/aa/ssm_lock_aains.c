#include <ssm_lock_ref.h>

int ssm_lock_aains(ssm_Iaa aa, void *key, void *value)
{
  int result;
  ssm_lockaa lockaa = (ssm_lockaa) aa;

  lockaa->lock->arm(lockaa->lock);
  result = lockaa->base->ins(lockaa->base, key, value);
  lockaa->lock->rel(lockaa->lock);

  return result;
}

