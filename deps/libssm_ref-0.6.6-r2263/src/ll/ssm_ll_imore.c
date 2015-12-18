#include <ssm_ll_ref.h>

int ssm_ll_imore(ssm_Ii i)
{
  ssm_lli lli = (ssm_lli) i;
  if(lli->nlln == NULL)
    return 0;
  return 1;
}
