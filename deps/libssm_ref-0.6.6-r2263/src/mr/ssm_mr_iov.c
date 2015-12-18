#include <ssm_mr_ref.h>

size_t ssm_mr_iov(struct iovec *iovs, ssm_mr mr, size_t offset, size_t length, int iovcount)
{
  int v = 0;
  if(length == 0) length = mr->info.span;
  char *p = mr->info.base;
  size_t result;
  ssm_md resultmd = ssm_md_sub(NULL, offset, length, mr->md);
  
  result = ssm_md_base(iovs, resultmd, mr->info.base, iovcount);
  ssm_md_release(resultmd);
  return result;
}
