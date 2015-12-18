#include <ssm_md_ref.h>

ssm_md ssm_md_release(ssm_md md)
{
  if(!md)
    return NULL;
  ssm_Ii i = md->mdns->i(md->mdns);
  while(i->next(i))
    SSM_DELETE(i->cur(i));
  i->del(i);
  md->mdns->del(md->mdns);
  SSM_DELETE(md);
  return NULL;
}
