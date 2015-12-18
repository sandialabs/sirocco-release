#ifndef __SSM_PP_FLAGS_H
#define __SSM_PP_FLAGS_H

#include <ssm/pp/base.h>

#define SSMPP_FIELD_I(r, prefix, i, symbol) \
  SSMPP_CAT(prefix, symbol) = (1 << i), 

#define SSMPP_FLAGS(nspace, cnspace, primitive, symbol, cprefix, ...) \
  typedef primitive SSMPP_CAT(nspace, _, symbol); \
  enum { \
    SSMPP_FEI(SSMPP_FIELD_I, SSMPP_CAT(cnspace, _, cprefix, _), __VA_ARGS__) \
  }

#endif
