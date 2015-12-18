#ifndef __SSM_PP_PRINT_H
#define __SSM_PP_PRINT_H

#include <ssm/pp/base.h>

/*
#define SSMPP_PRINT(prefix, fstr, ...) \
  printf("  [" prefix "|%25s:%3d] " fstr "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define SSMPP_PRINT_IF(cond, prefix, fstr, ...) \
  SSM_IIF(cond, \
      SSMPP_PRINT(prefix, fstr, __VA_ARGS__))
*/
#define SSMPP_PRINTPARAM_ARGS_END() 0
#define SSMPP_PRINTPARAM_STR_END(s) ""
#define SSMPP_PRINTPARAM_SEP_END(s) ""
#define SSMPP_PRINTPARAM_VAR_END(s) s
#define SSMPP_PRINTPARAM_PRE_END(s) 0

#define SSMPP_PRINTPARAM_ARGS_p() 1
#define SSMPP_PRINTPARAM_STR_p(s) SSMPP_STR(s)"=%p"
#define SSMPP_PRINTPARAM_SEP_p(s) ", "
#define SSMPP_PRINTPARAM_VAR_p(s) (void *)s
#define SSMPP_PRINTPARAM_PRE_p(s) 1

#define SSMPP_PRINTPARAM_ARGS_d() 1
#define SSMPP_PRINTPARAM_STR_d(s) SSMPP_STR(s)"=%d"
#define SSMPP_PRINTPARAM_SEP_d(s) ", "
#define SSMPP_PRINTPARAM_VAR_d(s) (unsigned int)s
#define SSMPP_PRINTPARAM_PRE_d(s) 1

#define SSMPP_PRINTPARAM_ARGS_x() 1
#define SSMPP_PRINTPARAM_STR_x(s) SSMPP_STR(s)"=%X"
#define SSMPP_PRINTPARAM_SEP_x(s) ", "
#define SSMPP_PRINTPARAM_VAR_x(s) (unsigned int)s
#define SSMPP_PRINTPARAM_PRE_x(s) 1

#define SSMPP_PRINTPARAM_ARGS_s() 1
#define SSMPP_PRINTPARAM_STR_s(s) SSMPP_STR(s)"=\"%s\""
#define SSMPP_PRINTPARAM_SEP_s(s) ", "
#define SSMPP_PRINTPARAM_VAR_s(s) (const char *)s
#define SSMPP_PRINTPARAM_PRE_s(s) 1

#define SSMPP_PRINTPARAM_ARGS_lit() 1
#define SSMPP_PRINTPARAM_STR_lit(s) "%s"
#define SSMPP_PRINTPARAM_SEP_lit(s) " "
#define SSMPP_PRINTPARAM_VAR_lit(s) (const char *)s
#define SSMPP_PRINTPARAM_PRE_lit(s) 1

#define SSMPP_PRINTPARAM_ARGS_lab() 1
#define SSMPP_PRINTPARAM_STR_lab(s) "%s"
#define SSMPP_PRINTPARAM_SEP_lab(s) ": "
#define SSMPP_PRINTPARAM_VAR_lab(s) (const char *)s
#define SSMPP_PRINTPARAM_PRE_lab(s) 1

#define SSMPP_PRINTPARAM_ARGS_pun() 1
#define SSMPP_PRINTPARAM_STR_pun(s) "%s"
#define SSMPP_PRINTPARAM_SEP_pun(s) ""
#define SSMPP_PRINTPARAM_VAR_pun(s) (const char *)s
#define SSMPP_PRINTPARAM_PRE_pun(s) 0

#define SSMPP_PRINTPARAM_ARGS_file() 0
#define SSMPP_PRINTPARAM_STR_file(s) "%25s"
#define SSMPP_PRINTPARAM_SEP_file(s) ""
#define SSMPP_PRINTPARAM_VAR_file(s) __FILE__
#define SSMPP_PRINTPARAM_PRE_file(s) 1

#define SSMPP_PRINTPARAM_ARGS_line() 0
#define SSMPP_PRINTPARAM_STR_line(s) "%3d"
#define SSMPP_PRINTPARAM_SEP_line(s) ""
#define SSMPP_PRINTPARAM_VAR_line(s) __LINE__
#define SSMPP_PRINTPARAM_PRE_line(s) 1

#define SSMPP_PRINTPARAM_ARGS_proc() 0
#define SSMPP_PRINTPARAM_STR_proc(s) "%s"
#define SSMPP_PRINTPARAM_SEP_proc(s) ""
#define SSMPP_PRINTPARAM_VAR_proc(s) __FUNCTION__
#define SSMPP_PRINTPARAM_PRE_proc(s) 1



#define SSMPP_PRINTPARAM_FUNCNAME(func, type) \
  SSMPP_CAT(SSMPP_PRINTPARAM_, func, _, type)

#define SSMPP_PRINTPARAM_FUNC(func, seq) \
  SSMPP_PRINTPARAM_FUNCNAME(func, SSMPP_SEQ_AT(seq, 0))

#define SSMPP_PRINTPARAM_ARGS(type) \
  SSMPP_CAT(SSMPP_PRINTPARAM_ARGS_, type)()

#define SSMPP_PRINTPARAM_SEQARGS(seq) \
  SSMPP_PRINTPARAM_ARGS(SSMPP_SEQ_AT(seq, 0))

#define SSMPP_PRINTPARAM_CALL(func, seq) \
  SSMPP_EXPAND(\
    SSMPP_PRINTPARAM_FUNC(func, seq) \
    SSMPP_SEQ_TUP(SSMPP_SEQ_SUB(seq, 1, SSMPP_PRINTPARAM_SEQARGS(seq)))\
  )


#define SSMPP_PRINT_PRED(r, state) \
  SSMPP_LT(1, SSMPP_SEQ_LEN(state))

#define SSMPP_PRINT_OP(r, state) \
  SSMPP_SEQ_REST(SSMPP_SEQ_REST(state, 1), \
      SSMPP_PRINTPARAM_SEQARGS(state))

#define SSMPP_PRINT_SEP(state) \
  SSMPP_IIF( \
    SSMPP_PRINTPARAM_CALL(PRE, SSMPP_PRINT_OP(r, state)), \
    SSMPP_PRINTPARAM_CALL(SEP, state))

#define SSMPP_PRINT_VARCOMMA(state) \
  SSMPP_COMMA_IF(SSMPP_LT(1, SSMPP_SEQ_LEN(state)))

#define SSMPP_PRINTPARAM_STRMACRO(r, state) \
  SSMPP_PRINTPARAM_CALL(STR, state)\
  SSMPP_PRINT_SEP(state)

#define SSMPP_PRINTPARAM_VARMACRO(r, state) \
  SSMPP_PRINT_VARCOMMA(state)\
  SSMPP_PRINTPARAM_CALL(VAR, state)

#define SSMPP_PUTS_DSEQ(seq) \
  printf(\
      SSMPP_FOR(seq, SSMPP_PRINT_PRED, SSMPP_PRINT_OP, SSMPP_PRINTPARAM_STRMACRO) "\n"\
      SSMPP_FOR(seq, SSMPP_PRINT_PRED, SSMPP_PRINT_OP, SSMPP_PRINTPARAM_VARMACRO))

#define SSMPP_PUTS_SEQ(seq) \
  SSMPP_PUTS_DSEQ(SSMPP_SEQ_ADDBOT(seq, END))

#define SSMPP_PUTS(...) \
  SSMPP_PUTS_SEQ(SSMPP_SEQ(__VA_ARGS__))

#define SSMPP_PRINT(...) \
  SSMPP_PUTS(__VA_ARGS__)

static inline void ssm_pp_testprint()
{
  int num = 234;
  SSMPP_PRINT(d, num);
}

#endif
