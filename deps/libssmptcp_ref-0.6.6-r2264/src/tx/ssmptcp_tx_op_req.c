#include <ssmptcp_ref.h>

int ssmptcp_tx_op_req(ssmptcp_tx tx, ssmptcp_conn conn, uint64_t a, uint64_t b)
{
  // ack'd other side, they're requesting data
  // we're not getting anything else, so just send
  tx->st = SSMPTCP_TX_SEND;
  tx->remote_call = SSMPTCP_CALL_PULL;
  tx->next_op = SSMPTCP_OP_DATA;
  return 0;
}
