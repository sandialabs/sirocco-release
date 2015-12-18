#include <ssmptcp_ref.h>

int ssmptcp_tx_op_ack(ssmptcp_tx tx, ssmptcp_conn conn, uint64_t a, uint64_t b)
{
  // other side acknowleded, so we're ready to REQ or DATA.
  // a = remote_id, b = ssmtpreply
  tx->remote_id = a;
  tx->ssmreply = b;
  tx->st = SSMPTCP_TX_SEND;
  if(tx->local_call == SSMPTCP_CALL_PUSH)
    tx->next_op = SSMPTCP_OP_DATA;
  else
    tx->next_op = SSMPTCP_OP_REQ;
  return 0;
}
