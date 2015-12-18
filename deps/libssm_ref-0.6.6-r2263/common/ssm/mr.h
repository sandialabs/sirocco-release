#ifndef SSM_MR_H
#define SSM_MR_H

#include <ssm/ssm_pp.h>
#include <stdint.h>
#include <sys/uio.h>
#include <ssm/md.h>
#include <ssm/if/i.h>
#include <ssm/md.h>
/* File:  ssm_mr.h
 *
 * Declares methods for an SSM Memory Record.
 */

SSM_OTYPE(mr);

SSM_TYPE(mrinfo)
{
  ssm_mdinfo_t mdinfo;
  ssm_md md;
  void *base;
  size_t span;
};

SSM_HANDLE(map);
SSM_FLAGS(map, MAP, HEAD, PERSIST);
SSM_TYPE(mmr)
{
  ssm_mr mr;
  ssm_Fmap flags;
  ssm_Hmap map;
};


extern ssm_mr ssm_mr_create(ssm_md md, void *base, size_t maxlen);
extern int ssm_mr_getinfo(ssm_mrinfo info, ssm_mr mr);
extern int ssm_mr_destroy(ssm_mr mr); 
extern size_t ssm_mr_iov(struct iovec *iovs, ssm_mr mr, size_t offset, size_t length, int iovcount);
extern size_t ssm_mr_mdiov(struct iovec *iovs, ssm_mr mr, ssm_md md, int iovcount);
extern size_t ssm_mr_cpmd(ssm_mr target, ssm_md targetmd, ssm_mr src, ssm_md srcmd);
extern int ssm_mr_free(ssm_mr mr);

static inline size_t ssm_mr_span(ssm_mr mr)
{
  ssm_dtrace();
  ssm_mrinfo_t mrinfo;
  ssm_mr_getinfo(&mrinfo, mr);
  return mrinfo.span;
}

static inline void * ssm_mr_base(ssm_mr mr)
{
  ssm_mrinfo_t mrinfo;
  ssm_mr_getinfo(&mrinfo, mr);
  return mrinfo.base;
}

static inline size_t ssm_mr_len(ssm_mr mr)
{
  ssm_mrinfo_t mrinfo;
  ssm_mr_getinfo(&mrinfo, mr);
  ssm_dprint(p, mrinfo.base, p, mrinfo.md, d, mrinfo.span, d, mrinfo.mdinfo.max, d, mrinfo.mdinfo.min, d, mrinfo.mdinfo.len);
  if(mrinfo.md)
    return mrinfo.mdinfo.len;
  else
    return mrinfo.span;
}

#endif
