#include <ssm_md_ref.h>

ssm_md ssm_md_addv(ssm_md md, struct iovec *vecs, int count)
{
  int i;
  for(i = 0; i < count; i++)
    md = ssm_md_add(md, (long)vecs[i].iov_base, vecs[i].iov_len);
  return md;
}
