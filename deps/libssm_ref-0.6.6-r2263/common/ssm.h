#ifndef _SSM_H
#define _SSM_H
/* ssm.h
 *  Main SSM API functions
 *
 * This is a program file for the Nebula project, subproject XYZ
 *
 * File author:      Shane Farmer, UAB, shanefarmer22@gmail.com
 * File maintainer:  Shane Farmer, UAB, shanefarmer22@gmail.com
 *
 * Prerelease status: only for distribution to Nebula project members for all project-related purposes
 *
 * Part or all of this file/work may be copyrighted by the Regents of the University of Alabama, and be subject
 * to their standard disclaimer of liability.  Such language will be added to the files before any public 
 * release.
 *
 */

/* File: ssm.h
 * Main SSM API.
 */
#include <stdint.h>
#include <stddef.h>
#include <sys/time.h>

#if 0
#include <ssm/ssmdef.h>
#include <ssm/ssmmem.h>
#include <ssm/ssma.h>
#include <ssm/ssmd.h>
#include <ssm/ssme.h>
#include <ssm/ssmm.h>
#include <ssm/ssmt.h>
#endif 

#include <ssm/pp.h>
#include <ssm/if.h>
#include <ssm/mr.h>
#include <ssm/tp.h>
#include <ssm/cb.h>
//#include <ssm/print.h>
//#include <ssm/output.h>

/* Constants:  Preprocessor directives
 *
 * Concurrency type directives:
 * SSMPP_CONC_NONE - Don't attempt concurrency (progress only made on ssm_wait).  [default]
 * SSMPP_CONC_PTHREADS - Use PTHREADS for concurrency.
 * SSMPP_CONC_RUNTIME - Use the SSMGLOBAL <conc_type> to decide at initialization-time.
 * 
 * Logging level directives:
 * SSMPP_OUT_VQUIET -  Suppress warnings, errors, and debugging.
 * SSMPP_OUT_QUIET -  Suppress warnings and debugging.
 * SSMPP_OUT_NORMAL -  Suppress debugging output only.  [default]
 * SSMPP_OUT_VERBOSE -  Suppress only verbose debugging messages.
 * SSMPP_OUT_VVERBOSE -  Enable all output.
 * SSMPP_OUT_RUNTIME -  Use the SSMGLOBAL <logging_level> to set the logging level during runtime.
 *
 * Assertion level directives:
 * SSMPP_ASSERT_NONE -  Disable all assertions.
 * SSMPP_ASSERT_ENABLE -  Enable basic assertions.  [default]
 * SSMPP_ASSERT_STRICT -  Enable strict assertions.
 * SSMPP_ASSERT_RUNTIME -  Use the SSMGLOBAL <assert_level> to set the assertion level.
 *
 * Formatting directives:
 * SSMPP_FORMAT_GNU - Use GNU formatting guidelines for output.
 * SSMPP_FORMAT_DEFAULT - Use fixed-size columns for output. [defualt]
 * SSMPP_FORMAT_CUSTOM - As SSMPP_FORMAT_DEFAULT, but use SSMGLOBALs to set column widths.
 * SSMPP_FORMAT_RUNTIME - Use the SSMGLOBAL <formatting_type> to determine output type.
 */

/* Type:  ssm_flags
 * SSM initialization flags.
 */
SSM_FLAGS(start, START, PLACEHOLDER);
SSM_FLAGS(link, LINK, INIT_CB, AUTO_UNLINK);
SSM_FLAGS(post, POST, STATIC, PLACEHOLDER);
SSM_FLAGS(op, OP, INIT_CB);

/* Types:  SSM Opaque Types
 *
 * ssm_id - SSM handle
 * ssm_me - SSM Match entry.  Represents a flat buffer and match/callback semantics registered with SSM.
 * ssm_tx - SSM transaction qualifier.  Represents an in-progress transaction; used for ssm_cancel.
 */
SSM_OTYPE(id, me, tx);

/* Enum:  ssm_pos_type
 * Types of position specifiers.
 *
 * SSM_POS_HEAD - Beginning of list
 * SSM_POS_TAIL - End of list
 * SSM_POS_BEFORE - Before a provided anchor
 * SSM_POS_AFTER - After a provided anchor
 */
SSM_ENUM(pos, POS, HEAD, TAIL, BEFORE, AFTER);

SSM_BENUM(op, OP, PUT, GET, PING, MSG, UNLINK);

SSM_BENUM(status, ST, NOMATCH, NOBUF, NETFAIL, REMOTE, DROP, CANCEL, COMPLETE);

SSM_PRIMITIVE(bits, uint64_t);

/* Event structure
 */
SSM_TYPE(result)
{
  ssm_id     id;
  ssm_me     me;
  ssm_tx     tx;
  ssm_bits   bits;
  ssm_status status;
  ssm_op     op;
  ssm_Haddr  addr;
  ssm_mr     mr;
  ssm_md     md;
  uint64_t   bytes;
};

/* Section:  SSM start/stop
 * Startup and teardown functions.
 */

extern ssm_id ssm_start(ssm_Itp tp, void *addr_args, ssm_Fstart flags);
extern int ssm_stop(ssm_id id);
extern ssm_Iaddr ssm_addr(ssm_id id);

extern ssm_me ssm_link(ssm_id id, ssm_bits bits, ssm_bits mask, ssm_pos pos, ssm_me anchor, ssm_cb cb, ssm_Flink flags);

SSM_ENUM(remove_result, REMOVE, OK, BUSY, INVALID, INVALID_ME, INVALID_BUF);
extern int ssm_unlink(ssm_id id, ssm_me me);
extern int ssm_drop(ssm_id id, ssm_me me, ssm_mr mr);

/* Section:  SSM MD interface
 * The default SSM interface using memory descriptors
 */

extern int ssm_post(ssm_id, ssm_me me, ssm_mr mr, ssm_Fpost flags);

extern ssm_tx ssm_put(ssm_id id, ssm_Haddr addr, ssm_mr src, ssm_md target, ssm_bits bits, ssm_cb cb, ssm_Fop flags);
extern ssm_tx ssm_get(ssm_id id, ssm_Haddr addr, ssm_md src, ssm_mr target, ssm_bits bits, ssm_cb cb, ssm_Fop flags);

/* Section:  SSM iovec interface
 * SSM operations using ivoecs
 */

extern int ssm_postv(ssm_id, ssm_me me, struct iovec *iovs, int len, ssm_Fop flags);
extern ssm_tx ssm_putv(ssm_id, ssm_Haddr addr, struct iovec *iovs, int len, ssm_bits bits, ssm_cb cb, ssm_Fop flags);
extern ssm_tx ssm_getv(ssm_id, ssm_Haddr addr, struct iovec *iovs, int len, ssm_bits bits, ssm_cb cb, ssm_Fop flags);

/* Section:  Other SSM Functions
 * Functions not memory specifit
 */

extern ssm_tx ssm_msg(ssm_id, ssm_Haddr addr, uint64_t a, uint64_t b, ssm_bits bits, ssm_cb cb, void *cb_data, ssm_Fop flags);
extern ssm_tx ssm_ping(ssm_id, ssm_Haddr addr, ssm_cb cb, void *cb_data);
extern int ssm_cancel(ssm_id, ssm_tx tx);
extern int ssm_wait(ssm_id, struct timeval *tv);

extern ssm_Haddr ssm_addr_create(ssm_id id, ssm_Haddrargs opts);
extern ssm_Haddr ssm_addr_cp(ssm_id id, ssm_Haddr src);
extern size_t ssm_addr_cpv(ssm_id id, ssm_Haddr src, ssm_Haddr *dest, int count);
extern ssm_Haddr ssm_addr_local(ssm_id id);
extern int ssm_addr_cmp(ssm_id id, ssm_Haddr a, ssm_Haddr b);
extern int ssm_addr_destroy(ssm_id id, ssm_Haddr target);
extern ssm_Haddr ssm_addr_load(ssm_id id, char *in, size_t len);
extern size_t ssm_addr_serialize(char *out, size_t len, ssm_id id, ssm_Haddr addr);
#endif
