#include <ssm_md_ref.h>

size_t ssm_md_serialize(void *vout, size_t len, ssm_md md)
{
  if(vout == NULL)
    return md->info.elems * 16;
  
  size_t off = 0;
  uint64_t *out = vout;
  ssm_Ii i = md->mdns->i(md->mdns);
  while(i->next(i) != NULL)
  {
    ssm_mdn node = i->cur(i);
    out[off  ] = (uint64_t) node->off;
    out[off+1] = (uint64_t) node->len;
    off += 2;
  }
  i->del(i);
  return off * 8;
}
