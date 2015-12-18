#ifndef __SSM_TP_IFSSM_H
#define __SSM_TP_IFSSM_H

#include <ssm/ssm_pp.h>
#include <ssm/mr.h>
#include <ssm/if/addr.h>

SSM_HANDLE(ssm, tx); // pointer to ssm state given when transport calls ssm functions

SSM_BENUM(tpreply, TPREPLY, NOMATCH, NOBUF, CANCEL, MATCH);
SSM_BENUM(tpact, TPACT, WROTE, READ, FAILED);

// response created by ssm when transport posts trasnfer head
SSM_TYPE(tpr)
{
  ssm_Htx tx;  // transaction handle for the transport to use with finish
  ssm_mmr mmr; // mr describing where to put transfer tail
  ssm_tpreply reply;
};

// Functions provided by SSM to the transport

SSM_INTERFACE_DECL(host);

// posts the head of a trasnfer and gets an ssm_tpresponse written
SSM_PROC(int, hostpost)(ssm_Ihost host, ssm_Haddr addr, ssm_Il responses, ssm_Il head);

// completes a transaction that involved posting
SSM_PROC(int, hostrelease)(ssm_Ihost host, ssm_Htx tx, ssm_md md, ssm_tpreply reply, ssm_tpact action);

// finishes a transaction (likely need some descriptors here)
SSM_PROC(int, hostfinish)(ssm_Ihost host, ssm_Htx tx, ssm_md md, ssm_tpreply reply, ssm_tpact action);

SSM_PROC(int, hostdel)(ssm_Ihost host);

// interface for transport to use when calling ssm
SSM_INTERFACE_DEFN(host)
{
  ssm_Phostpost post;
  ssm_Phostrelease release;
  ssm_Phostfinish finish;
  ssm_Phostdel del;
};


#endif
