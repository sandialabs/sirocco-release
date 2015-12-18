#include <ssm_ll_ref.h>

int ssm_ll_laddbef(ssm_Il l, void *d, void *anchor)
{
  ssm_ll ll = (ssm_ll)l;
  ssm_lln node = SSM_NEW(lln);
  node->data = d;
  node->owner = ll;

  if(ll->head == NULL)
  {
    ll->head = node;
    node->next = NULL;
  }
  else
  {
    ssm_lln cur = ll->head;
    node->next = node;
    while(node->next == node)
    {
      if(cur->next == NULL)
      {
        cur->next = node;
        ll->tail = node;
        node->next = NULL;
      }
      else if(cur->next->data == anchor)
      {
        node->next = cur->next;
        cur->next = node;
      }
      else 
        cur = cur->next;
    }
  }
  ll->len++;
  return 0;
}

