#include <ssm_lock_ref.h>

ssm_Ii ssm_lock_i(ssm_Ilock lock, ssm_Ii i)
{
  ssm_locki locki = SSM_NEW(locki);

  locki->i.cur = ssm_lock_icur;
  locki->i.next = ssm_lock_inext;
  locki->i.more = ssm_lock_imore;
  locki->i.del  = ssm_lock_idel;
  locki->base = i;
  locki->lock = lock;

  return (ssm_Ii)locki;
}
