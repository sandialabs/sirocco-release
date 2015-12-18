#ifndef __SSM_TP_IFADDR_H
#define __SSM_TP_IFADDR_H

#include <ssm/ssm_pp.h>

SSM_HANDLE(addr, addrargs); 

// address functions (transport would provide these)
SSM_INTERFACE_DECL(addr);

// create address
SSM_PROC(ssm_Haddr, addrcreate)(ssm_Iaddr addr, ssm_Haddrargs opts);

// copy address
SSM_PROC(ssm_Haddr, addrcp)(ssm_Iaddr addr, ssm_Haddr source);

// make multiple copies of address
SSM_PROC(size_t, addrcpv)(ssm_Iaddr addr, ssm_Haddr source, ssm_Haddr *dest, int count);

// get an address that routs to local
SSM_PROC(ssm_Haddr, addrlocal)(ssm_Iaddr addr);

// compare two addresses
SSM_PROC(int, addrcmp)(ssm_Iaddr addr, ssm_Haddr a, ssm_Haddr b);

// release an address
SSM_PROC(int, addrdestroy)(ssm_Iaddr addr, ssm_Haddr target);

SSM_PROC(size_t, addrserialize)(ssm_Iaddr addr, char *out, size_t len, ssm_Haddr target);
SSM_PROC(ssm_Haddr, addrload)(ssm_Iaddr addr, char *in, size_t len);

SSM_PROC(int, addrdel)(ssm_Iaddr addr);

// interface populated by the transport with their functions
SSM_INTERFACE_DEFN(addr)
{
  ssm_Paddrcreate create;
  ssm_Paddrcp cp;
  ssm_Paddrcpv cpv;
  ssm_Paddrcmp cmp;
  ssm_Paddrlocal local;
  ssm_Paddrdestroy destroy;
  ssm_Paddrserialize serialize;
  ssm_Paddrload load;
  ssm_Paddrdel del;
};


#endif
