#ifndef __SSM_PBUF_H
#define __SSM_PBUF_H

#include <ssm/ssm_pp.h>

#define SSM_PBUF_LEN 10

typedef char ssm_pbuf[SSM_PBUF_LEN];

static inline void ssm_pbuf_store(char *buf, void *ptr)
{
  *((void **)buf) = ptr;
}
static inline void *ssm_pbuf_fetch(char *buf)
{
  return *((void **)buf);
}

static inline void ssm_pbuf_cp(char *dst, char *src)
{
  memmove(dst, src, SSM_PBUF_LEN);
}

#endif
