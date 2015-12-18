/* Usage
 *
 * Before including, define LOG_PREFIX to put a name to your log
 * outputs.  Declare "int log_lvl" in the same file as your main
 * function, and assign it to a value. Use the first parameter of INFO
 * to describe verbosity. Use 0 as non-conditional output, 1 for
 * startup/shutdown, 2 for per-operation.
 */

#ifndef _LOGGING_H_
#define _LOGGING_H_

#include <sys/types.h>

/* Suggested values for log_lvl */
#define INFO_ALWAYS 0 /* Always log */
#define INFO_ONCE   1 /* Logged once per execution */
#define INFO_N      2 /* Logged once per operation */
#define INFO_MANY   3 /* Logged several times per operation, e.g. polling */

#ifndef LOG_PREFIX
#define LOG_PREFIX ""
#endif

extern int log_lvl;
extern pid_t PID;

#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)

#define LOG_PRINT(prepend, fmt, ...)	((void)fprintf(stderr, "[ %i ] " LOG_PREFIX " " prepend " @" __FILE__ ":" STRINGIFY(__LINE__) "\t" fmt "%s", PID, __VA_ARGS__))

#define WARN(...) LOG_PRINT("warning", __VA_ARGS__, "")

#define ERR(...)  LOG_PRINT("error  ", __VA_ARGS__, "")

#define INFO(lvl, ...) do { if (lvl <= log_lvl) { LOG_PRINT("info   ", __VA_ARGS__, ""); } } while(0)

#endif
