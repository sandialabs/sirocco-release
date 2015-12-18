#ifndef __SSM_PP_BENUM_H
#define __SSM_PP_BENUM_H
#if 0
#include <ssm/pp/name.h>
#include <ssm/pp/deco.h>

#define SSM_BENUM_PRIMITIVE uint32_t

#define SSM_BENUM_VAL_ITER(n, prefix, i, symbol) \
  SSM_CPNAME(prefix, symbol) =  (1 << i),

#define SSM_ENUM_STR_ITER(r, prefix, symbol) \
  case SSM_CPNAME(prefix, symbol): \
    return SSM_STR(SSM_CPNAME(prefix, symbol)); break;

#define SSM_BENUM(symbol, constprefix, valseq) \
  typedef enum SSM_HNAME(symbol) { \
    BOOST_PP_SEQ_FOR_EACH_I(SSM_BENUM_VAL_ITER, constprefix, valseq) \
  } SSM_NAME(symbol);\
  static inline const char * SSM_CAT((SSM_NAME(symbol))(_str))(int inputval) { \
    switch(inputval) { SSM_SEQ_FE(SSM_ENUM_STR_ITER, constprefix, valseq) \
      default: return "Unknown value";} }

#endif
#endif
