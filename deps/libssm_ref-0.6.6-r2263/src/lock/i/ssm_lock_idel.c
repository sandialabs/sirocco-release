#include <ssm_lock_ref.h>

int ssm_lock_idel(ssm_Ii i)
{
  ssm_locki locki = (ssm_locki)i;
  int result;
  result = locki->base->del(locki->base);
  locki->lock->rel(locki->lock);
  SSM_DEL(locki);
  return result;
}



