#ifndef __SSM_PP_BUILD_H
#define __SSM_PP_BUILD_H

// build options
#ifndef SSMPP_OUT_VQUIET
  #ifndef SSMPP_OUT_QUIET
    #ifndef SSMPP_OUT_NORMAL
      #ifndef SSMPP_OUT_VERBOSE
        #ifndef SSMPP_OUT_VVERBOSE
          #ifndef SSMPP_OUT_RUNTIME
            #define SSMPP_OUT_NORMAL
          #endif
        #endif
      #endif
    #endif
  #endif
#endif

#ifdef SSMPP_OUT_RUNTIME
  #warn "SSMPP_OUT_RUNTIME not currently supported, using SSMPP_OUT_NORMAL"
  #define SSMPP_OUT_NORMAL
#endif

#ifdef SSMPP_OUT_VQUIET
// do nothing
#endif

#ifdef SSMPP_OUT_QUIET
  #define SSMPP_BUILD_ERRORS 1
#endif

#ifdef SSMPP_OUT_NORMAL
  #define SSMPP_BUILD_ERRORS 1
  #define SSMPP_BUILD_WARNINGS 1
#endif

#ifdef SSMPP_OUT_VERBOSE
  #define SSMPP_BUILD_ERRORS 1
  #define SSMPP_BUILD_WARNINGS 1
  #define SSMPP_BUILD_DEBUG 1
#endif

#ifdef SSMPP_OUT_VVERBOSE
  #define SSMPP_BUILD_ERRORS 1
  #define SSMPP_BUILD_WARNINGS 1
  #define SSMPP_BUILD_DEBUG 1
  #define SSMPP_BUILD_VERBOSEDEBUG 1
#endif

#ifndef SSMPP_BUILD_ERRORS
  #define SSMPP_BUILD_ERRORS 0
#endif
#ifndef SSMPP_BUILD_WARNINGS
  #define SSMPP_BUILD_WARNINGS 0
#endif
#ifndef SSMPP_BUILD_DEBUG
  #define SSMPP_BUILD_DEBUG 0
#endif
#ifndef SSMPP_BUILD_VERBOSEDEBUG
  #define SSMPP_BUILD_VERBOSEDEBUG 0
#endif

#ifndef SSMPP_ASSERT_NONE
  #ifndef SSMPP_ASSERT_ENABLE
    #ifndef SSMPP_ASSERT_STRICT
      #ifndef SSMPP_ASSERT_RUNTIME
        #define SSMPP_ASSERT_ENABLE
      #endif
    #endif
  #endif
#endif

#ifdef SSMPP_ASSERT_RUNTIME
  #warn "SSMPP_ASSERT_RUNTIME not implemented yet, using SSMPP_ASSERT_ENABLE"
  #define SSMPP_ASSERT_ENABLE
#endif

#ifdef SSMPP_ASSERT_NONE
// do nothing
#endif

#ifdef SSMPP_ASSERT_ENABLE
  #define SSMPP_BUILD_ASSERTS 1
#endif

#ifdef SSMPP_ASSERT_STRICT
  #define SSMPP_BUILD_STRICTASSERTS 1
#endif

#ifndef SSMPP_BUILD_ASSERTS
  #define SSMPP_BUILD_ASSERTS 0
#endif
#ifndef SSMPP_BUILD_STRICTASSERTS
  #define SSMPP_BUILD_STRICTASSERTS 0
#endif

#endif
