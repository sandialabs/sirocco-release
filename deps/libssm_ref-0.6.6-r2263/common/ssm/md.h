#ifndef SSM_MD_H
#define SSM_MD_H

#include <ssm/ssm_pp.h>
#include <ssm/if/i.h>
#include <ssm/if/l.h>
#include <stdint.h>
#include <sys/uio.h>

/* File:  ssm_md.h
 *
 * Declares methods for an SSM Memory Descriptor.
 */

typedef uint32_t ssm_md_flags;

SSM_OTYPE(md);

SSM_TYPE(mdinfo)
{
  size_t min;
  size_t max;
  size_t len;
  int elems;
};

extern ssm_md ssm_md_add(ssm_md md, size_t off, size_t len);
extern ssm_md ssm_md_addv(ssm_md md, struct iovec *vecs, int count);
extern ssm_md ssm_md_release(ssm_md md);

extern size_t  ssm_md_serialize(void *out, size_t len, ssm_md md);
extern ssm_md ssm_md_load(void *data, size_t datalen);
extern size_t ssm_md_base(struct iovec *out, ssm_md md, char *base, int iovcount);

extern ssm_md ssm_md_cat(ssm_md head, ssm_md tail);
extern ssm_md ssm_md_sub(ssm_md target, size_t off, size_t len, ssm_md src);
extern ssm_md ssm_md_split(ssm_md *left, ssm_md *right, ssm_md md, size_t len);
extern ssm_md ssm_md_copy(ssm_md md);

extern int ssm_md_getinfo(ssm_mdinfo info, ssm_md md);
extern ssm_Ii ssm_md_i(ssm_md md);



#endif
