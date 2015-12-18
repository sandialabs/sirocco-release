#ifndef __SSM_PP_TYPE_H
#define __SSM_PP_TYPE_H

#include <ssm/pp/name.h>

#define SSMPP_TYPE_HNAME(nspace, symbol) \
  SSMPP_CAT(SSMPP_PREFIX, nspace, _, symbol)

#define SSMPP_TYPE_VNAME(nspace, symbol) \
  SSMPP_CAT(nspace, _, symbol, _t)

#define SSMPP_TYPE_PNAME(nspace, symbol) \
  SSMPP_CAT(nspace, _, symbol)

#define SSMPP_TYPE_NAME(nspace, symbol) \
  SSMPP_CAT(nspace, _, symbol)

#define SSMPP_TYPE_DECL(nspace, symbol) \
  struct SSMPP_TYPE_HNAME(nspace, symbol); \
  typedef struct SSMPP_TYPE_HNAME(nspace, symbol) SSMPP_TYPE_VNAME(nspace, symbol); \
  typedef struct SSMPP_TYPE_HNAME(nspace, symbol) * SSMPP_TYPE_PNAME(nspace, symbol)

#define SSMPP_TYPE_DEFN(nspace, symbol) \
  struct SSMPP_TYPE_HNAME(nspace, symbol)

#define SSMPP_TYPE(nspace, symbol) \
  SSMPP_TYPE_DECL(nspace, symbol); \
  SSMPP_TYPE_DEFN(nspace, symbol)

#define SSMPP_TYPE_SIZEOF_NAME(nspace, symbol) \
  SSMPP_CAT(nspace, _, symbol, _sizeof)

#define SSMPP_TYPE_SIZEOF_DECL(nspace, symbol) \
  size_t SSMPP_TYPE_SIZEOF_NAME(nspace, symbol)()

#define SSMPP_TYPE_SIZEOF_DEFN(nspace, symbol) \
  size_t SSMPP_TYPE_SIZEOF_NAME(nspace, symbol)() \
    {return sizeof(struct SSMPP_TYPE_HNAME(nspace, symbol))}\
  SSMPP_EAT_SEMICOLON

#endif
