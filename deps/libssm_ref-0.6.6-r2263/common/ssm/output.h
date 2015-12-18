#ifndef SSM_OUTPUT_H
#define SSM_OUTPUT_H

#include <ssm/print.h>
// SSM output (logging, debugging, etc.) functions
// Matthew Shane Farmer
//

#define SSM_STRINGIFY(x) #x
#define SSM_TOSTRING(x) SSM_STRINGIFY(x)

#define SSM_PREFIX_LEN 35
#define SSM_OUTPUT_LEN 80

#define SSM_PREFIX_E "EE!"
#define SSM_PREFIX_W "ww>"
#define SSM_PREFIX_D ">"

#define ssm_log(prefix, ...) \
  ssm_print("RrrL",          \
            3, prefix,       \
            4, SSM_TOSTRING(__LINE__), \
            SSM_PREFIX_LEN - (3 + 4 + 1), __func__, \
            SSM_OUTPUT_LEN - (SSM_PREFIX_LEN + 2), __VA_ARGS__)

#define ssm_log_e(...) ssm_log(SSM_PREFIX_E, __VA_ARGS__)
#define ssm_log_w(...) ssm_log(SSM_PREFIX_W, __VA_ARGS__)
#define ssm_log_d(...) ssm_log(SSM_PREFIX_D, __VA_ARGS__)


#define SSMPP_SHOW_WARNINGS
#define SSMPP_SHOW_ERRORS

#ifdef SSMPP_OUT_ALL
  #define SSMPP_SHOW_ERRORS
  #define SSMPP_SHOW_WARNINGS
  #define SSMPP_SHOW_DEBUG
#endif

#ifdef SSMPP_SHOW_NO_ERRORS
  #undef SSMPP_SHOW_ERRORS
#endif

#ifdef SSMPP_SHOW_NO_WARNINGS
  #undef SSMPP_SHOW_WARNINGS
#endif

#ifdef SSMPP_SHOW_NOTHING
  #undef SSMPP_SHOW_ERRORS
  #undef SSMPP_SHOW_WARNINGS
  #undef SSMPP_SHOW_DEBUG
#endif

#ifdef SSMPP_SHOW_ERRORS
  #define ssm_eprintf(...) ssm_log_e(__VA_ARGS__)
#else
  #define ssm_eprintf(...)
#endif

#ifdef SSMPP_SHOW_WARNINGS
  #define ssm_wprintf(...) ssm_log_w(__VA_ARGS__)
#else
  #define ssm_wprintf(...)
#endif

#ifdef SSMPP_SHOW_DEBUG
  #define ssm_dprintf(...) ssm_log_d(__VA_ARGS__)
#else
  #define ssm_dprintf(...)
#endif


#define SSM_ABORT() do {fflush(NULL); abort();} while(0)

#define SSM_INNER_ASSERT(type, expr) \
  do{if(!(expr)) {ssm_log("ABT", "%s '%s' failed", type, #expr ); SSM_ABORT();}}while(0)


#ifdef SSMPP_ASSERT_ALL
  #define SSMPP_ASSERT_PARAMS
  #define SSMPP_ASSERT_RUNTIME
#endif

#ifdef SSMPP_ASSERT_NONE
  #undef SSMPP_ASSERT_PARAMS
  #undef SSMPP_ASSERT_RUNTIME
#endif

#ifdef SSMPP_ASSERT_NO_PARAMS
  #undef SSMPP_ASSERT_PARAMS
#endif

#ifdef SSMPP_ASSERT_NO_RUNTIME
  #undef SSMPP_ASSERT_RUNTIME
#endif

#ifdef SSMPP_ASSERT_PARAMS
  #define ssm_passert(expr) SSM_INNER_ASSERT("param assert", expr)
#else
  #define ssm_passert(expr)
#endif

#ifdef SSMPP_ASSERT_RUNTIME
  #define ssm_rassert(expr) SSM_INNER_ASSERT("runtime assert", expr)
#else
  #define ssm_rassert(expr)
#endif

#endif
