#ifndef __SSM_PP_ENUM_H
#define __SSM_PP_ENUM_H

#include <ssm/pp/name.h>
#include <ssm/pp/deco.h>


#define SSMPP_ENUM_I(r, prefix, i, symbol) \
  SSMPP_CAT(prefix, symbol),

#define SSMPP_ENUM_IBIT(r, prefix, i, symbol) \
  SSMPP_CAT(prefix, symbol) = (1 << i),

#define SSMPP_ENUM_INEG(r, prefix, i, symbol) \
  SSMPP_CAT(prefix, symbol) = (0 - i),

#define SSMPP_ENUMSTR_I(r, prefix, symbol) \
  case SSMPP_CAT(prefix, symbol): \
    return SSMPP_STR(SSMPP_CAT(prefix, symbol)); break;

#define SSMPP_ENUM(iter, nspace, cnspace, symbol, cprefix, ...) \
  typedef enum SSMPP_CAT(SSMPP_PREFIX, nspace, _, symbol) { \
    SSMPP_FEI(iter, SSMPP_CAT(cnspace, _, cprefix, _), __VA_ARGS__) \
  } SSMPP_CAT(nspace, _, symbol);\
  static inline const char * SSMPP_CAT(nspace, _, symbol, _, str)(int inputval) { \
    switch(inputval) { \
      SSMPP_FE(SSMPP_ENUMSTR_I, SSMPP_CAT(cnspace, _, cprefix, _), __VA_ARGS__) \
      default: return "Unknown value";} }


#endif
