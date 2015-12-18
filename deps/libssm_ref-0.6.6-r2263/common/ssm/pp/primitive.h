#ifndef __SSM_PP_PRIMITIVE_H
#define __SSM_PP_PRIMITIVE_H

#include <ssm/pp/name.h>

#define SSMPP_PRIMITIVE(nspace, prim, name) \
  typedef prim SSMPP_CAT(nspace, _, name);

#endif
