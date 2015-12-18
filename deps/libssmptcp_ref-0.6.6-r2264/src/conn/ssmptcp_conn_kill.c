#include <ssmptcp_ref.h>

void ssmptcp_conn_kill(ssmptcp_conn conn)
{
  if(conn->tp == NULL)
  {
    ssm_wprint(lit, "Tried to kill inactive conn", p, conn);
    return;
  }
  ssm_dtrace(p, conn);
  ssmptcp_tx tx;
  ssmptcp_epdel(conn->tp, conn);
  conn->tp->active--;
  close(conn->fd);

  tx = conn->txs->remtop(conn->txs);
  while(tx)
  {
    ssmptcp_tx_abort(tx);
    ssmptcp_tx_finish(tx, conn->tp->host);
    SSM_DEL(tx);
    tx = conn->txs->remtop(conn->txs);
  }

  conn->txs->del(conn->txs);
  conn->tp->cleanup_conns->addtop(conn->tp->cleanup_conns, conn);
  /* This should be redundant; added for #609 */
  conn->tp = NULL;
}
