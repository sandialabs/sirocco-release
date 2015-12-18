#include <ssmptcp_ref.h>

ssm_Iaddr ssmptcp_tpaddr(ssm_Itp tp)
{
  ssmptcp_iaddr result = SSMPTCP_NEW(iaddr);

  result->addr.cp = ssmptcp_addrcp;
  result->addr.cpv = ssmptcp_addrcpv;
  result->addr.cmp = ssmptcp_addrcmp;
  result->addr.create = ssmptcp_addrcreate;
  result->addr.local = ssmptcp_addrlocal;
  result->addr.destroy = ssmptcp_addrdestroy;
  result->addr.del = ssmptcp_addrdel;
  result->addr.serialize = ssmptcp_addr_serialize;
  result->addr.load = ssmptcp_addr_load;
  result->tp = (ssmptcp_tp)tp;

  return (ssm_Iaddr) result;
}
