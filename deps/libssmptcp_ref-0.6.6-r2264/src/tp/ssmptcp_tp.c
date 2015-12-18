#include <ssmptcp_ref.h>

ssm_Itp ssmptcp_new_tp(int port, ssmptcp_Fconn flags)
{
  ssmptcp_tp tp = SSMPTCP_NEW(tp);
  tp->tp.start = ssmptcp_tpstart;
  tp->tp.stop = ssmptcp_tpstop;
  tp->tp.map = ssmptcp_tpmap;
  tp->tp.unmap = ssmptcp_tpunmap;
  tp->tp.push = ssmptcp_tppush;
  tp->tp.pull = ssmptcp_tppull;
  tp->tp.addr = ssmptcp_tpaddr;
  tp->tp.wait = ssmptcp_tpwait;
  tp->tp.wake = ssmptcp_tpwake;
  tp->tp.del = ssmptcp_tpdel;
  tp->host = NULL;
  tp->active = 0;
  tp->port = htons(port);
  tp->headmaxlen = 0;
  tp->connfd = 0;
  return (ssm_Itp)tp;
}
