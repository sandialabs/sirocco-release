#include <ssm_md_ref.h>

ssm_md ssm_md_add(ssm_md md, size_t off, size_t len)
{
  if(md == NULL)
    return ssm_md_new(off, len);
  else
  {
    ssm_mdn node = SSM_NEW(mdn);
    node->off = off;
    node->len = len;
    md->info.max = SSM_MAX(md->info.max, len + off);
    md->info.len += len;
    md->info.min = SSM_MIN(md->info.min, off);
    md->info.elems++;
    md->mdns->addbot(md->mdns, node);
    return md;
  }
}
