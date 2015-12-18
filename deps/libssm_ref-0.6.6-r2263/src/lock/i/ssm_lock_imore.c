#include <ssm_lock_ref.h>

int ssm_lock_imore(ssm_Ii i)
{
  ssm_locki locki = (ssm_locki)i;
  return locki->base->more(locki->base);
}

