#include <ssm_mr_ref.h>

ssm_mr ssm_mr_create(ssm_md md, void *base, size_t span)
{
  ssm_mr result = SSM_NEW(mr);
  result->md = md;
  result->info.base = base;
  result->info.span = span;
  result->info.md = md;
  ssm_md_getinfo(&(result->info.mdinfo), md);
  return result;
}
