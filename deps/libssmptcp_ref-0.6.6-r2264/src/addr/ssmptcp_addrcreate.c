#include <ssmptcp_ref.h>

ssm_Haddr ssmptcp_addrcreate(ssm_Iaddr addr, ssm_Haddrargs vargs)
{
  ssmptcp_addrargs args = vargs;
  ssm_dtrace(p, addr, p, args, d, args->port, s, args->host);

  ssmptcp_addr result = SSMPTCP_NEW(addr);
  result->port = htons(args->port);
  memmove(result->host, args->host, 20);
  result->origin = SSMPTCP_ADDR_LOCAL;
  return result; 
}
