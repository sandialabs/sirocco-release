#include <ssm_ll_ref.h>

int ssm_ll_ldel(ssm_Il l)
{
  ssm_ll ll = (ssm_ll)l;
  ssm_lln node = ll->head;
  ssm_lln d;
  while(node != NULL)
  {
    d = node;
    node = node->next;
    SSM_DELETE(d);
  }

  SSM_DELETE(ll);
  return 0;
}

