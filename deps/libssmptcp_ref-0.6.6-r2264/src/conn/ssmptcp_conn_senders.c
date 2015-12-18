#include <ssmptcp_ref.h>

static void * ssmptcp_conn_senders_fe(void *vconn, void *vtx)
{
  ssmptcp_conn conn = vconn;
  ssmptcp_tx tx = vtx;
  ssm_ddprint(lit, "checking for ready", s, ssmptcp_op_str(tx->next_op), s, ssmptcp_tx_st_str(tx->st));
  if(ssmptcp_tx_ready(tx))
    return tx;
  return NULL;
}

int ssmptcp_conn_senders(ssmptcp_conn conn)
{
  int r = 1;
  if(ssm_i_foreach(conn->txs->i(conn->txs), ssmptcp_conn_senders_fe, conn) == NULL)
    r = 0;
  ssm_dprint(d, r);
  return r;
}
