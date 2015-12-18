#include <ssm_ll_ref.h>

void * ssm_ll_lrembot(ssm_Il l)
{
  ssm_ll ll = (ssm_ll)l;
  void *result;

  result = l->bot(l);
  int r = l->rem(l, result);
  return result;
}
