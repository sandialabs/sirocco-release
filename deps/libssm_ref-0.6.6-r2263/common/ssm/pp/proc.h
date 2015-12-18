#ifndef __SSM_PP_FPTR_H
#define __SSM_PP_FPTR_H

#include <ssm/pp/name.h>
#include <ssm/pp/deco.h>

#define SSMPP_PROC(nspace, ret, name) \
  typedef ret (* SSMPP_CAT(nspace, _, name))

#endif
