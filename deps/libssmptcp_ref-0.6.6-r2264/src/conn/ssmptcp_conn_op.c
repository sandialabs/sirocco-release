#include <ssmptcp_ref.h>

static void *ssmptcp_conn_op_fe(void *vid, void *vtx)
{
  // find the tx associated with an ID
  // this is O(n) on the number of txs, i.e. bad
  int *pid = vid;
  ssmptcp_tx tx = vtx;
  if(tx->id == *pid)
    return tx;
  return NULL;
}

int ssmptcp_conn_op(ssmptcp_conn conn, ssmptcp_op op, uint32_t id, uint64_t a, uint64_t b)
{
  if(op == SSMPTCP_OP_RPORT)
    return ssmptcp_conn_op_rport(conn, a, b);
  else if(op == SSMPTCP_OP_PAIR)
    return ssmptcp_conn_op_pair(conn, a, b);
  
  ssmptcp_tx tx = ssm_i_foreach(conn->txs->i(conn->txs), ssmptcp_conn_op_fe, &id); 
  if(tx == NULL)
  {
    ssm_wprint(lit, "dropping packet", d, id);
    ssmptcp_tx_drop(conn, op, a, b);
  }
  else
  {
    ssmptcp_tx_op(tx, conn, op, a, b);
  }
  return 0;

}
