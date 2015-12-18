#ifndef __SSM_PP_MEM_H
#define __SSM_PP_MEM_H

#include <ssm/pp/name.h>
#include <ssm/pp/print.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/uio.h>

#define SSM_MALLOC(arg)\
  malloc(arg)

#define SSM_TMALLOC(arg) \
  SSM_MALLOC(sizeof(arg))

#define SSM_MALLOCV(arg, count) \
  calloc(count, arg)

#define SSM_TMALLOCV(arg, count) \
  SSM_MALLOCV(count, sizeof(arg))

#define SSM_NEW(type) \
  SSM_TMALLOC(SSM_VNAME(type))

#define SSM_NEWV(type, y) \
  SSM_TMALLOCV(SSM_VNAME(type), y)

#define SSM_FREE(x) \
  free(x)

#define SSM_DEL(x) SSM_FREE(x)

#define SSM_DELETE(x) SSM_DEL(x)

#define SSM_MIN(x,y) ((x) < (y) ? (x) : (y))
#define SSM_MAX(x,y) ((x) > (y) ? (x) : (y))

#define SSM_SIZEOF(type) \
  sizeof(SSM_VNAME(type))


#endif
