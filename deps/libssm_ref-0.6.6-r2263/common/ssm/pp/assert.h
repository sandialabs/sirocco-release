#ifndef __SSM_PP_ASSERT_H
#define __SSM_PP_ASSERT_H

#include <stdio.h>

#define SSMPP_ABORT() do { fflush(NULL); abort(); } while(0)

#define SSMPP_ASSERT(nspace, type, expr) \
  do{if(!(expr)) {printf("%s:%d: ABORT: %s %s '%s' failed\n", __FILE__, __LINE__,\
      nspace, type, #expr ); SSMPP_ABORT();}}while(0)

#define SSMPP_ASSERT_IF(nspace, type, expr) \
  SSM_IIF(cond, SSMPP_ASSERT(nspace, type, expr), SSMPP_EMPTY)

#define SSMPP_ASSERT_I(r, data, expr) \
  SSMPP_ASSERT(SSMPP_SEQ_AT(data, 0), SSMPP_SEQ_AT(data, 1), expr);

#define SSMPP_ASSERTS(nspace, type, ...) \
  SSMPP_FE(SSMPP_ASSERT_I, (nspace)(type), __VA_ARGS__)
  

#endif
