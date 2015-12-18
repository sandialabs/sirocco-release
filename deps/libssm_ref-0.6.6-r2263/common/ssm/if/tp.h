#ifndef __SSM_TP_IFTP_H
#define __SSM_TP_IFTP_H

#include <ssm/ssm_pp.h>
#include <ssm/if/host.h>
#include <ssm/if/addr.h>

// functions provided by the transport to SSM

SSM_HANDLE(tp); // transport state given with all transport calls
SSM_FLAGS(tpmap, MAP, PINS);

SSM_TYPE(tpinfo)
{
  ssm_Ftpmap mapflags;
  size_t maplimit;
};


SSM_INTERFACE_DECL(tp);

// inits and provides information about the transport
SSM_PROC(int, tpstart)(ssm_Itp tp, ssm_Ihost host, ssm_tpinfo reply, size_t headmaxlen);

// stops
SSM_PROC(int, tpstop)(ssm_Itp tp);

// maps (pins/regs/etc.) an MR; ssm saves the returned state
SSM_PROC(int, tpmap)(ssm_Itp tp, ssm_Il mmrs);

// unmaps an MR
SSM_PROC(int, tpunmap)(ssm_Itp tp, ssm_Il mmrs);

// gets data from here to there.  'head' is control data that must be sent before
// the transport will know where to put the rest (although they're free to presend some of tail,
// too).  tail is the remainder of the data (the user buffer, for instance).  writemd is an MD 
// describing how to write the data at the target.
SSM_PROC(int, tppush)(ssm_Itp tp, ssm_Il txs, ssm_Haddr addr, ssm_Il head, ssm_Il mmrs, ssm_Il mds);

// gets data from there to here.  head is as before and must be sent to other side.  target is the 
// local tpmr that will receive the data.  readmd tells the other side how to read the data.
SSM_PROC(int, tppull)(ssm_Itp tp, ssm_Il txs, ssm_Haddr addr, ssm_Il head, ssm_Il mmrs, ssm_Il mds);

// gets an address interface
SSM_PROC(ssm_Iaddr, tpaddr)(ssm_Itp tp);

// Delivers a thread of execution to the transport for a specified number of microseconds, -1 for until progress
SSM_PROC(int, tpwait)(ssm_Itp tp, long usec);

// Signals that if the transport is blocked in a waiting thread, it should wake.
SSM_PROC(int, tpwake)(ssm_Itp tp);

// Deletes (deinitializes) the interface.  How the transport splits this with stop is up to the trasport.
SSM_PROC(int, tpdel)(ssm_Itp tp);


// interface for ssm to use when calling transport
SSM_INTERFACE_DEFN(tp)
{
  ssm_Ptpstart start;
  ssm_Ptpstop stop;
  ssm_Ptpmap map;
  ssm_Ptpunmap unmap;
  ssm_Ptppush push;
  ssm_Ptppull pull;
  ssm_Ptpaddr addr;
  ssm_Ptpwait wait;
  ssm_Ptpwake wake;
  ssm_Ptpdel del;
};




#endif
