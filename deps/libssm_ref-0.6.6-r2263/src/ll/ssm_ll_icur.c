#include <ssm_ll_ref.h>

void * ssm_ll_icur(ssm_Ii i)
{
  ssm_lli lli = (ssm_lli)i;
  return lli->cur;
}

