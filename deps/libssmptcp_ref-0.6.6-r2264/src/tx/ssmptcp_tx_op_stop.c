#include <ssmptcp_ref.h>

int ssmptcp_tx_op_stop(ssmptcp_tx tx, ssmptcp_conn conn, uint64_t a, uint64_t b)
{
  tx->st = SSMPTCP_TX_DONE;
  tx->ssmreply = b;
}
