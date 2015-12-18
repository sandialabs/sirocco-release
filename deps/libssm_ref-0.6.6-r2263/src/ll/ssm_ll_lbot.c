#include <ssm_ll_ref.h>

void *ssm_ll_lbot(ssm_Il l)
{
  ssm_ll ll = (ssm_ll)l;
  ssm_lln n = ll->tail;
  if(n == NULL)
    return NULL;
  return n->data;
}
