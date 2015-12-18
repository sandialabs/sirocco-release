#ifndef __SSM_SSM_PP_H
#define __SSM_SSM_PP_H

#include <ssm/pp.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/uio.h>

/* Printing */

#define ssm_pprint(prefix, ...) \
  SSMPP_PRINT(\
      pun, "  [", \
      pun, prefix,\
      pun, "|",\
      file,\
      pun, ":",\
      line,\
      pun, "] ",\
      __VA_ARGS__)

#define ssm_print(...) \
  ssm_pprintf("nfo", __VA_ARGS__)

#define ssm_eprint(...) \
  SSMPP_IIF(SSMPP_BUILD_ERRORS, ssm_pprint("ERR", __VA_ARGS__))

#define ssm_wprint(...) \
  SSMPP_IIF(SSMPP_BUILD_WARNINGS, ssm_pprint("Wrn", __VA_ARGS__))

#define ssm_dprint(...) \
  SSMPP_IIF(SSMPP_BUILD_DEBUG, ssm_pprint("   ", __VA_ARGS__))

#define ssm_ddprint(...) \
  SSMPP_IIF(SSMPP_BUILD_VERBOSEDEBUG, ssm_pprint("   ", __VA_ARGS__))

#define ssm_trace(...) \
  ssm_pprint("-->",\
      proc,\
      pun, "(",\
      SSMPP_INLINE_ARGS(__VA_ARGS__)\
      pun, ")")

#define ssm_return(...) \
  ssm_pprint("<--",\
      proc,\
      lit, " returning: ",\
      __VA_ARGS__)

#define ssm_dtrace(...) \
  SSMPP_IIF(SSMPP_BUILD_DEBUG, ssm_trace(__VA_ARGS__))

#define ssm_ddtrace(...) \
  SSMPP_IIF(SSMPP_BUILD_VERBOSEDEBUG, ssm_trace(__VA_ARGS__))

#define ssm_dreturn(...) \
  SSMPP_IIF(SSMPP_BUILD_DEBUG, ssm_return(__VA_ARGS__))

#define ssm_ddreturn(...) \
  SSMPP_IIF(SSMPP_BUILD_VERBOSEDEBUG, ssm_return(__VA_ARGS__))

/* Asserts */

#define ssm_assert(type, ...) \
  SSMPP_ASSERTS("ssm", type, __VA_ARGS__)

#define ssm_passert(...) \
  ssm_assert("param", __VA_ARGS__)

#define ssm_ppassert(...) \
  ssm_assert("strict param", __VA_ARGS__)

#define ssm_rassert(...) \
  ssm_assert("runtime", __VA_ARGS__)

#define ssm_rrassert(...) \
  ssm_assert("strict runtime", __VA_ARGS__)

/* Types */

#define SSM_PRIMITIVE(symbol, type) \
  SSMPP_PRIMITIVE(ssm, type, symbol)

#define SSM_NO_FLAGS 0
#define SSM_NOF 0
#define SSM_FLAGS(symbol, cprefix, ...) \
  SSMPP_FLAGS(ssm, SSM, uint64_t, SSMPP_CAT(F, symbol), cprefix, __VA_ARGS__)

#define SSM_HANDLE_I(r, data, symbol) \
  SSM_PRIMITIVE(SSMPP_CAT(H, symbol), void *)

#define SSM_HANDLE(...) \
  SSMPP_FE(SSM_HANDLE_I, unused, __VA_ARGS__);\
SSMPP_EAT_SEMICOLON

#define SSM_ENUM(symbol, cprefix, ...) \
  SSMPP_ENUM(SSMPP_ENUM_I, ssm, SSM, symbol, cprefix, __VA_ARGS__)

#define SSM_BENUM(symbol, cprefix, ...) \
  SSMPP_ENUM(SSMPP_ENUM_IBIT, ssm, SSM, symbol, cprefix, __VA_ARGS__)

#define SSM_RENUM(symbol, cprefix, ...) \
  SSMPP_ENUM(SSMPP_ENUM_INEG, ssm, SSM, symbol, cprefix, __VA_ARGS__)

#define SSM_TYPE_DECL_I(r, data, symbol) \
  SSMPP_TYPE_DECL(ssm, symbol);

#define SSM_TYPE_DECL(...) \
  SSMPP_FE(SSM_TYPE_DECL_I, unused, __VA_ARGS__)\
SSMPP_EAT_SEMICOLON

#define SSM_TYPE_DEFN(symbol) \
  SSMPP_TYPE_DEFN(ssm, symbol)

#define SSM_TYPE(symbol) \
  SSMPP_TYPE(ssm, symbol)

#define SSM_OTYPE_I(r, data, symbol) \
  SSMPP_TYPE_DECL(ssm, symbol); \
SSMPP_TYPE_SIZEOF_DECL(ssm, symbol);

#define SSM_OTYPE(...) \
  SSMPP_FE(SSM_OTYPE_I, unused, __VA_ARGS__) \
SSMPP_EAT_SEMICOLON

#define SSM_OTYPE_DEFN(symbol) \
  SSMPP_TYPE_DEFN(ssm, symbol)

#define SSM_INTERFACE_I(r, data, symbol) \
  SSMPP_TYPE_DECL(ssm, SSMPP_CAT(I, symbol)); \
SSMPP_EAT_SEMICOLON

#define SSM_INTERFACE_DECL(...) \
  SSMPP_FE(SSM_INTERFACE_I, unused, __VA_ARGS__)

#define SSM_INTERFACE_DEFN(symbol) \
  SSMPP_TYPE_DEFN(ssm, SSMPP_CAT(I, symbol))

#define SSM_PROC(ret, symbol) \
  SSMPP_PROC(ssm, ret, SSMPP_CAT(P, symbol))

/* Memory */
#define SSM_MALLOC(arg)\
  malloc(arg)

#define SSM_TMALLOC(arg) \
  SSM_MALLOC(sizeof(arg))

#define SSM_MALLOCV(arg, count) \
  calloc(count, arg)

#define SSM_TMALLOCV(arg, count) \
  SSM_MALLOCV(count, sizeof(arg))

#define SSM_NEW(type) \
  SSM_TMALLOC(SSMPP_TYPE_VNAME(ssm, type))

#define SSM_NEWV(type, y) \
  SSM_TMALLOCV(SSMPP_TYPE_VNAME(ssm, type), y)

#define SSM_FREE(x) \
  free(x)

#define SSM_DEL(x) SSM_FREE(x)

#define SSM_DELETE(x) SSM_DEL(x)

#define SSM_MIN(x,y) ((x) < (y) ? (x) : (y))
#define SSM_MAX(x,y) ((x) > (y) ? (x) : (y))

#define SSM_SIZEOF(type) \
  sizeof(SSMPP_TYPE_VNAME(ssm, type))


static inline int ssm_iovcopy(struct iovec *dest, int destlen, struct iovec *src, int srclen)
{
  ssm_dtrace(p, dest, d, destlen, p, src, d, srclen);
  char *r, *w;
  size_t rl, wl, tl;

  r = src->iov_base;
  rl = src->iov_len;
  w = dest->iov_base;
  wl = dest->iov_len;
  while(srclen && destlen)
  {
    tl = SSM_MIN(rl, wl);
    ssm_dprint(lab, "pre-write", p, r, d, rl, p, w, d, wl, d, tl, d, srclen, d, destlen);
    memmove(w, r, tl);
    rl -= tl;
    r += tl;
    wl -= tl;
    w += tl;
    if(!rl)
    {
      src++;
      srclen--;
      if(srclen)
      {
        r = src->iov_base;
        rl = src->iov_len;
      }
    }
    if(!wl)
    {
      dest++;
      destlen--;
      if(destlen)
      {
        w = dest->iov_base;
        wl = dest->iov_len;
      }
    }
    ssm_dprint(lab, "post-write", p, r, d, rl, p, w, d, wl, d, tl, d, srclen, d, destlen);
  }
  return 0;
}


#include <ssm/mem.h>

#endif
