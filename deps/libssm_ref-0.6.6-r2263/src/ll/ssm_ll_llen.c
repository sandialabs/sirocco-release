#include <ssm_ll_ref.h>

size_t ssm_ll_llen(ssm_Il l)
{
  ssm_ll ll = (ssm_ll)l;
  return ll->len;
}
