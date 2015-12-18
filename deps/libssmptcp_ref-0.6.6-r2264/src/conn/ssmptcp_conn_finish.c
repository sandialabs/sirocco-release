#include <ssmptcp_ref.h>

static void * ssmptcp_conn_finish_fe(void *vhost, void *vtx)
{
  ssm_Ihost host = vhost;
  ssmptcp_tx tx = vtx;
  if(ssmptcp_tx_done(tx))
  {
    ssmptcp_tx_finish(tx, host);
    return tx;
  }
  return NULL;
}

int ssmptcp_conn_finish(ssmptcp_conn conn)
{
  // this is REALLY ugly, n log n behavior
  ssmptcp_tx dead_tx;
  dead_tx = ssm_i_foreach(conn->txs->i(conn->txs), ssmptcp_conn_finish_fe, conn->tp->host);
  while(dead_tx != NULL)
  {
    conn->txs->rem(conn->txs, dead_tx);
    ssmptcp_tx_kill(dead_tx);
    dead_tx = ssm_i_foreach(conn->txs->i(conn->txs), ssmptcp_conn_finish_fe, conn->tp->host);
  }

  return 0;
}
