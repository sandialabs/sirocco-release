#include <ssm_ll_ref.h>

int ssm_ll_lrem(ssm_Il l, void *ptr)
{
  ssm_ll ll = (ssm_ll)l;
  int result = -1;
  
  ssm_lln cur = ll->head;
  ssm_lln prv = NULL;
  while(result == -1)
  {
    if(cur== NULL)
    {
      result = 0;
    }
    else if(cur->data == ptr)
    {
      if(prv == NULL)
      {
        ll->head = ll->head->next;
        SSM_DELETE(cur);
      }
      else
      {
        prv->next = cur->next;
        if(ll->tail == cur)
          ll->tail = prv;
        SSM_DELETE(cur);
      }
      result = 1;
      ll->len--;
    }
    else
    {
      prv = cur;
      cur = cur->next;
    }
  }
  return result;
}
