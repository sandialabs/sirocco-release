#include <ssm_md_ref.h>

ssm_Ii ssm_md_i(ssm_md md)
{
  ssm_passert(md);
  ssm_dtrace();
  return md->mdns->i(md->mdns);
}
