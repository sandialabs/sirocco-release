#include <ssm_ll_ref.h>

void *ssm_ll_ltop(ssm_Il l)
{
  ssm_ll ll = (ssm_ll)l;
  void *d;
  if(ll->head == NULL)
    d = NULL;
  else
    d = ll->head->data;
  return d;
}
