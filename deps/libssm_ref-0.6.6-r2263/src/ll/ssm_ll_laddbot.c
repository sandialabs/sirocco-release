#include <ssm_ll_ref.h>

int ssm_ll_laddbot(ssm_Il l, void *d)
{
  ssm_ll ll = (ssm_ll)l;
  ssm_lln node = SSM_NEW(lln);
  node->data = d;
  node->owner = ll;

  if(ll->head == NULL)
  {
    ll->head = node;
    ll->tail = node;
    node->next = NULL;
  }
  else
  {
    ssm_lln cur = ll->tail;
    cur->next = node;
    node->next = NULL;
    ll->tail = node;
  }

  ll->len++;
  return 0;
}

