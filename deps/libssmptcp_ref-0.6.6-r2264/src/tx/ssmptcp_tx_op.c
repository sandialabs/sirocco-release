#include <ssmptcp_ref.h>

int ssmptcp_tx_op(ssmptcp_tx tx, ssmptcp_conn conn, ssmptcp_op op, uint64_t a, uint64_t b)
{
  ssm_dtrace(p, tx, p, conn, s, ssmptcp_op_str(op), d, a, d, b);
  if(tx->st == SSMPTCP_TX_DONE || tx->st == SSMPTCP_TX_FAIL)
    return ssmptcp_tx_drop(conn, op, a, b);

  switch(op)
  {
    case SSMPTCP_OP_MD: return ssmptcp_tx_op_md(tx, conn, a, b);
    case SSMPTCP_OP_DATA: return ssmptcp_tx_op_data(tx, conn, a, b);
    case SSMPTCP_OP_REQ: return ssmptcp_tx_op_req(tx, conn, a, b);
    case SSMPTCP_OP_DONE: return ssmptcp_tx_op_done(tx, conn, a, b);
    case SSMPTCP_OP_ACK: return ssmptcp_tx_op_ack(tx, conn, a, b);
    case SSMPTCP_OP_STOP: return ssmptcp_tx_op_stop(tx, conn, a, b);
    default:
      ssm_wprint(lit, "Received unsupported op", s, ssmptcp_op_str(op));
      return -1;
  }
}
