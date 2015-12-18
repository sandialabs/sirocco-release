#ifndef __SSM_PP_HANDLE_H
#define __SSM_PP_HANDLE_H

#include <ssm/pp/name.h>
#include <ssm/pp/deco.h>

#define SSM_HANDLE(symbol) \
  typedef void * SSM_PNAME(SSM_DECO_HANDLE, symbol)

#define SSM_CHANDLE(symbol) \
  typedef void * const SSM_PNAME(SSM_DECO_CHANDLE, symbol)

#endif
