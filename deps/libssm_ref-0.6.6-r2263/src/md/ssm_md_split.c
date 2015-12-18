#include <ssm_md_ref.h>

ssm_md ssm_md_split(ssm_md *left, ssm_md *right, ssm_md md, size_t len)
{
  *left = NULL;
  *right = NULL;

  if(len > md->info.max)
  {
    *left = md;
    *right = NULL;
    return NULL;
  }

  ssm_mdn node;
  ssm_Ii i = md->mdns->i(md->mdns);
  while(i->next(i) != NULL)
  {
    node = i->cur(i);
    if(node->off + node->len < len)
    {
      *left = ssm_md_add(*left, node->off, node->len);
    }
    else if(node->off > len)
    {
      *right = ssm_md_add(*right, node->off - len, node->len);
    }
    else
    {
      *left = ssm_md_add(*left, node->off, len - node->off);
      *right = ssm_md_add(*right, 0, node->len - (len - node->off));
    }
  }
  i->del(i);
  ssm_md_release(md);
  return NULL;
}
