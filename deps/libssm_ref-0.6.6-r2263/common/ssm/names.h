#ifndef __SSM_NAMES_H
#define __SSM_NAMES_H

#include <ssm/pp.h>

#define SSM_NSPACE ssm
#define SSM_CNSPACE SSM

#define SSM_TYPE_DECL(name) SSMPP_TYPE_DECL(SSM_NSPACE, name)
#define SSM_TYPE_DEFN(name) SSMPP_TYPE_DEFN(SSM_NSPACE, name)
#define SSM_TYPE(name) \
  SSM_TYPE_DECL(name);\
  SSM_TYPE_DEFN(name)

#define SSM_FLAGS(name, cprefix, ...) \
  SSMPP_FLAGS(SSM_NSPACE, SSM_CNSPACE, uint32_t, SSMPP_CAT(F, name), cprefix, __VA_ARGS__)

#define SSM_OTYPE(name) \
  SSMPP_TYPE_SIZEOF_DECL(SSM_NSPACE, name);\
  SSMPP_TYPE_DECL(name)

#define SSM_OTYPE_DEFN(name) \
  SSMPP_TYPE_DEFN(SSM_NSPACE, name)

#define SSM_HANDLE(name) \
  SSMPP_PRIMITIVE(SSM_NSPACE, void *, SSMPP_CAT(H, name));

#endif
