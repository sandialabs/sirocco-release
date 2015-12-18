#include <ssmptcp_ref.h>

int ssmptcp_tpstop(ssm_Itp itp)
{
  ssmptcp_tp tp = (ssmptcp_tp) itp;
  tp->conns->del(tp->conns);
  if(tp->active)
  {
    ssm_wprint(lit, "active conns after cache flush", d, tp->active);
  }
  while(tp->active)
    ssmptcp_tpwait(itp, -1);
  close(tp->epfd);
  while(tp->txs->len(tp->txs) > 0)
  {
    ssmptcp_tx tx = tp->txs->remtop(tp->txs);
    ssmptcp_tx_kill(tx);
  }
  tp->txs->del(tp->txs);
  tp->fail_conns->del(tp->fail_conns);
  tp->cleanup_conns->del(tp->cleanup_conns);
  SSM_FREE(tp->epevs);
  return 0;
}
