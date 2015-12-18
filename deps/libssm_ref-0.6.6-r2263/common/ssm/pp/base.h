#ifndef __SSM_PP_BASE_H
#define __SSM_PP_BASE_H

#define BOOST_PP_VARIADICS 1

#include <ssm/ext/boost/preprocessor.hpp>

// basic meta macros
// currently implemented using boost
// please convert any boost references in other files to use the symbols in
// this file instead; this helps with isolation

#define SSMPP_SEQ(...) \
  BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)

#define SSMPP_NIL \
  BOOST_PP_NIL

#define SSMPP_SEQ_CAT(seq) \
  BOOST_PP_SEQ_CAT(seq)

#define SSMPP_CAT(...) \
  SSMPP_SEQ_CAT(SSMPP_SEQ(__VA_ARGS__))

#define SSMPP_STR(str) \
  BOOST_PP_STRINGIZE(str)

#define SSMPP_IIF(a, b) \
  BOOST_PP_EXPR_IIF(a, b)

#define SSMPP_AND(a, b) \
  BOOST_PP_AND(a, b)

#define SSMPP_EQ(a, b) \
  BOOST_PP_EQUAL(a, b)

#define SSMPP_GT(a, b) \
  BOOST_PP_GREATER(a, b)

#define SSMPP_LT(a, b) \
  BOOST_PP_LESS(a, b)

#define SSMPP_LE(a, b) \
  BOOST_PP_LESS_EQUAL(a, b)

#define SSMPP_FOR(state, pred, op, macro) \
  BOOST_PP_FOR(state, pred, op, macro)

#define SSMPP_EVAL(val) \
  BOOST_PP_APPLY(val)

#define SSMPP_EXPAND(expr) \
  BOOST_PP_EXPAND(expr)

#define SSMPP_LEN(...) \
  BOOST_PP_VARIADIC_SIZE(__VA_ARGS__)

#define SSMPP_SEQ_AT(seq, i) \
  BOOST_PP_SEQ_ELEM(i, seq)

#define SSMPP_SEQ_LEN(seq) \
  BOOST_PP_SEQ_SIZE(seq)

#define SSMPP_SEQ_SUB(seq, i, len) \
  BOOST_PP_SEQ_SUBSEQ(seq, i, len)

#define SSMPP_SEQ_REST(seq, chop) \
  BOOST_PP_SEQ_REST_N(chop, seq)

#define SSMPP_SEQ_ADDBOT(seq, item) \
  BOOST_PP_SEQ_PUSH_BACK(seq, item)

#define SSMPP_TUP_AT(size, tup, i) \
  BOOST_PP_TUPLE_ELEM(size, i, tuple)

#define SSMPP_TUP_STRIP(tup) \
  BOOST_PP_TUPLE_REM_CTOR(tup)

#define SSMPP_SEQ_TUP(seq) \
  BOOST_PP_SEQ_TO_TUPLE(seq)

#define SSMPP_SEQ_FE(macro, data, seq) \
  BOOST_PP_SEQ_FOR_EACH(macro, data, seq)

#define SSMPP_SEQ_FEI(macro, data, seq) \
  BOOST_PP_SEQ_FOR_EACH_I(macro, data, seq)

#define SSMPP_FE(macro, data, ...) \
  SSMPP_SEQ_FE(macro, data, SSMPP_SEQ(__VA_ARGS__))

#define SSMPP_FEI(macro, data, ...) \
  SSMPP_SEQ_FEI(macro, data, SSMPP_SEQ(__VA_ARGS__))

#define SSMPP_PREFIX \
  ssmpp_hidden_

#define SSMPP_EAT_SEMICOLON
//  do{}while(0)

#define SSMPP_EMPTY \
  BOOST_PP_EMPTY()

#define SSMPP_BOOL(expr) \
  BOOST_PP_BOOL(expr)

#define SSMPP_COMMA \
  BOOST_PP_COMMA()

#define SSMPP_COMMA_IF(cond) \
  BOOST_PP_COMMA_IF(cond)

#define SSMPP_INLINE_ARGS(...) \
  __VA_ARGS__ SSMPP_COMMA_IF(SSMPP_GT(SSMPP_LEN(placeholder,##__VA_ARGS__), 1))
#endif
