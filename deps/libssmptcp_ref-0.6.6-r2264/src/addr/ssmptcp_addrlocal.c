#include <ssmptcp_ref.h>

ssm_Haddr ssmptcp_addrlocal(ssm_Iaddr addr)
{
  ssmptcp_iaddr iaddr = (ssmptcp_iaddr) addr;
  ssmptcp_addrargs_t args;
  args.port = iaddr->tp->port;
  args.host = "127.0.0.1";
  return addr->create(addr, &args);
}


