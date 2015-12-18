#include <ssm_md_ref.h>

ssm_md ssm_md_copy(ssm_md md)
{
  ssm_md result = NULL;
  ssm_mdn node;
  ssm_Ii i = md->mdns->i(md->mdns);
  while(i->next(i))
  {
    node = i->cur(i);
    result = ssm_md_add(result, node->off, node->len);
  }
  i->del(i);
  return result;
}
