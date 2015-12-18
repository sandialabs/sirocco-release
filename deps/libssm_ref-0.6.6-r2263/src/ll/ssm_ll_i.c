#include <ssm_ll_ref.h>

ssm_Ii ssm_ll_i(ssm_Il l)
{
  ssm_ll ll = (ssm_ll)l;
  ssm_lli result = SSM_NEW(lli);
  result->nlln = ll->head;
  result->owner = ll;
  result->cur = NULL;
  result->i.cur = ssm_ll_icur;
  result->i.next = ssm_ll_inext;
  result->i.more = ssm_ll_imore;
  result->i.del = ssm_ll_idel;
  return (ssm_Ii)result;
}
