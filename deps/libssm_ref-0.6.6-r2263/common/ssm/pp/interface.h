#ifndef __SSM_PP_INTERFACE_H
#define __SSM_PP_INTERFACE_H

#include <ssm/pp/name.h>


#define SSM_INTERFACE_DECL(interface) \
  SSM_PTYPE_DECL(SSM_DECO_INTERFACE, interface)

#define SSM_INTERFACE_DEFN(interface) \
  SSM_PTYPE_DEFN(SSM_DECO_INTERFACE, interface)

#define SSM_INTERFACE(interface) \
  SSM_PTYPE(SSM_DECO_INTERFACE, interface)

#endif
