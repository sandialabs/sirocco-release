#include <ssm_lock_ref.h>

ssm_Iaa ssm_lock_aa(ssm_Ilock lock, ssm_Iaa aa)
{
  ssm_lockaa lockaa = SSM_NEW(lockaa);

  lockaa->aa.ins = ssm_lock_aains;
  lockaa->aa.rem = ssm_lock_aarem;
  lockaa->aa.find = ssm_lock_aafind;
  lockaa->aa.del = ssm_lock_aadel;
  lockaa->base = aa;
  lockaa->lock = lock;

  return (ssm_Iaa)lockaa;
}

