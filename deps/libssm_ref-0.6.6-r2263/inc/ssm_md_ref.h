#ifndef SSM_MD_REF_H
#define SSM_MD_REF_H

#include <ssm/pp.h>
#include <ssm/md.h>
#include <ssm/ll.h>

SSM_TYPE(mdn)
{
  size_t off;
  size_t len;
};

SSM_OTYPE_DEFN(md)
{
  ssm_Il mdns;
  ssm_mdinfo_t info;
};

static inline ssm_md ssm_md_new(size_t off, size_t len)
{
  ssm_md result = SSM_NEW(md);
  result->mdns = ssm_ll_l(SSM_NOF);
  ssm_mdn init = SSM_NEW(mdn);
  init->off = off;
  init->len = len;
  result->info.min = off;
  result->info.max = len + off;
  result->info.len = len;
  result->info.elems = 1;
  result->mdns->addbot(result->mdns, init);
  return result;
}

#endif
