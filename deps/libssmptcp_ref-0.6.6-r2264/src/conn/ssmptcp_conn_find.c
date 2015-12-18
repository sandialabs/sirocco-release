#include <ssmptcp_ref.h>

ssmptcp_conn ssmptcp_conn_find(ssmptcp_tp tp, ssmptcp_addr addr)
{
  ssm_dtrace(p, tp, p, addr, s, addr->host, d, addr->port);

  ssmptcp_conn result = tp->conns->find(tp->conns, addr);
  if(result && result->st == SSMPTCP_CONN_READY)
  {
    ssm_dprint(lit, "found conn", p, result);
    return result;
  }
  else if(result)
  {
    ssm_wprint(lit, "found conn not in READY state", s, ssmptcp_conn_st_str(result->st));
  }
  return ssmptcp_conn_init(tp, addr); 
}
