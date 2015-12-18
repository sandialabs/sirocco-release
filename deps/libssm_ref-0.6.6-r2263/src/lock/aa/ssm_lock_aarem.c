#include <ssm_lock_ref.h>

void * ssm_lock_aarem(ssm_Iaa aa, void *key)
{
  void *result;
  ssm_lockaa lockaa = (ssm_lockaa) aa;

  lockaa->lock->arm(lockaa->lock);
  result = lockaa->base->rem(lockaa->base, key);
  lockaa->lock->rel(lockaa->lock);

  return result;
}
