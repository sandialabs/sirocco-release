#include <ssm_ll_ref.h>

int ssm_ll_lhas(ssm_Il l, void *d)
{
  ssm_ll ll = (ssm_ll)l;
  int result = -1;
  ssm_lln cur = ll->head;
  while(result == -1)
  {
    if(cur == NULL)
      result = 0;
    else if(cur->data == d)
      result = 1;
    else
      cur = cur->next;
  }
  return result;
}
