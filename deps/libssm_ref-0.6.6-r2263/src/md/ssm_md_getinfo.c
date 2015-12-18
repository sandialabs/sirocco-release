#include <ssm_md_ref.h>

int ssm_md_getinfo(ssm_mdinfo info, ssm_md md)
{
  if(!md)
  {
    info->min = 0;
    info->max = 0;
    info->len = 0;
    info->elems = 0;
  }
  else
  {
    info->min = md->info.min;
    info->max = md->info.max;
    info->elems = md->info.elems;
    info->len = md->info.len;
  }
  return 0;
}
