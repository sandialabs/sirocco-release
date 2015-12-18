#include <ssm_md_ref.h>

ssm_md ssm_md_cat(ssm_md head, ssm_md tail)
{
  ssm_Ii i = tail->mdns->i(tail->mdns);
  ssm_mdn node;
  while(i->next(i))
  {
    node = i->cur(i);
    head = ssm_md_add(head, node->off, node->len);
  }
  i->del(i);
  ssm_md_release(tail);
  return head;
}
