#include <ssm_mr_ref.h>

size_t ssm_mr_mdiov(struct iovec *iovs, ssm_mr mr, ssm_md md, int iovcount)
{
  size_t v = 0;
  size_t tv = 0;
  size_t *s;
  if(md)
  {
    ssm_Ii i = ssm_md_i(md);
    while(i->next(i))
    {
      s = i->cur(i);
      v = ssm_mr_iov(iovs, mr, s[0], s[1], iovcount);
      tv += v;
      if(iovs)
        iovs += v;
    }
    i->del(i);
  }
  else
  {
    tv = ssm_mr_iov(iovs, mr, 0, 0, iovcount);
  }
    return tv;
}
