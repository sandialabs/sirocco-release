/* Header used to make C files compile as AESOP */
#define USE_AESOP

/* Valgrind macros don't jive with Aesop compiler, so no extended valgrind
   support */
#undef USE_VALGRIND

struct mmsghdr;
#include <aesop/aesop.h>
