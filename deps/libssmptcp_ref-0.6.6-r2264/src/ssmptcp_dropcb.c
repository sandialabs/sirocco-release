#include <ssmptcp_ref.h>

void ssmptcp_dropcb(void *cbdat, void *evdat)
{
  ssm_dtrace(p, cbdat, p, evdat);
  ssmptcp_conn conn = evdat;
  ssmptcp_conn_kill(conn);
}
