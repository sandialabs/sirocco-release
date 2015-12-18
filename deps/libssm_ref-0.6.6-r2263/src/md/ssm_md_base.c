#include <ssm_md_ref.h>

size_t ssm_md_base(struct iovec *out, ssm_md md, char *base, int iovcount)
{
  ssm_mdn n;
  size_t v = 0;

  if(!out || !iovcount)
    return md->info.elems;
  ssm_Ii i = md->mdns->i(md->mdns);
  

  while(i->next(i) && iovcount)
  {
    n = i->cur(i);
    out[v].iov_base = base + n->off;
    out[v].iov_len = n->len;
    iovcount--;
    v++;
  }
  i->del(i);
  return v;
}
