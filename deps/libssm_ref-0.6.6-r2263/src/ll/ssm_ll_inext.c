#include <ssm_ll_ref.h>

void * ssm_ll_inext(ssm_Ii i)
{
  ssm_lli lli = (ssm_lli)i;
  if(lli->nlln == NULL)
    lli->cur = NULL;
  else
  {
    lli->cur = lli->nlln->data;
    lli->nlln = lli->nlln->next;
  }
  return lli->cur;
}
