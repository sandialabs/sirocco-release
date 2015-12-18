#include <ssm_lock_ref.h>

void * ssm_lock_aafind(ssm_Iaa aa, void *key)
{
  void *result;
  ssm_lockaa lockaa = (ssm_lockaa) aa;

  lockaa->lock->share(lockaa->lock);
  result = lockaa->base->find(lockaa->base, key);
  lockaa->lock->rel(lockaa->lock);

  return result;
}

