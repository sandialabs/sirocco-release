#include <ssm_ll_ref.h>

int ssm_ll_idel(ssm_Ii i)
{
  ssm_lli lli = (ssm_lli) i;
  SSM_DELETE(lli);
  return 0;
}
