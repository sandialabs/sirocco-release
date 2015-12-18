#include <ssm_ll_ref.h>

void * ssm_ll_lremtop(ssm_Il l)
{
  ssm_ll ll = (ssm_ll)l;
  void *result;

  result = l->top(l);
  l->rem(l, result);
  return result;
}
