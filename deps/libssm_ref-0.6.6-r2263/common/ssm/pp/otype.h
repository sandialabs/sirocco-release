#ifndef __SSM_PP_OTYPE_H
#define __SSM_PP_OTYPE_H

#include <ssm/ext/boost/preprocessor.hpp>
#include <ssm/pp/name.h>
#include <ssm/pp/type.h>

#define SSM_OTYPE_SIZEOF(symbol) \
  size_t BOOST_PP_CAT(SSM_NAME(symbol), SSM_DECO_SIZEOF_SUFFIX)()

#define SSM_OTYPE(symbol) \
  SSM_OTYPE_SIZEOF(symbol); \
  SSM_TYPE_DECL(symbol)

#define SSM_OTYPE_DEFN(symbol) \
  SSM_TYPE_DEFN(symbol)

#define SSM_OTYPE_SIZEOF_DEFN_ITER(r, prefix, symbol) \
  SSM_OTYPE_SIZEOF(symbol) { return sizeof(SSM_VNAME(symbol));}

#define SSM_OTYPE_SIZEOFS(symbolseq) \
  BOOST_PP_SEQ_FOR_EACH(SSM_OTYPE_SIZEOF_DEFN_ITER, 0, symbolseq)

#endif
