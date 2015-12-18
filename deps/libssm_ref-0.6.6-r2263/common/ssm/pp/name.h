#ifndef __SSM_PP_NAME_H
#define __SSM_PP_NAME_H

#include <ssm/pp/base.h>
#include <ssm/pp/deco.h>

#define SSM_NAME(symbol)  \
  SSM_CAT((SSM_PP_NAMESPACE)(symbol))  

#define SSM_PNAME(prefix, symbol) \
  SSM_CAT((SSM_PP_NAMESPACE)(prefix)(symbol))

#define SSM_VNAME(symbol) \
  SSM_CAT((SSM_PP_NAMESPACE)(symbol)(SSM_DECO_VALUE_SUFFIX))  

#define SSM_HNAME(symbol) \
  SSM_CAT((SSM_DECO_HIDDEN_PREFIX)(SSM_PP_NAMESPACE)(symbol))

#define SSM_CPNAME(cpnprefix, cpnsymbol) \
  SSM_CAT((SSM_PP_CNAMESPACE)(cpnprefix)(_)(cpnsymbol))

#endif
