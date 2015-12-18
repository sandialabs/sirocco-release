#include <ssmptcp_ref.h>

int ssmptcp_tx_op_done(ssmptcp_tx tx, ssmptcp_conn conn, uint64_t a, uint64_t b)
{
  // done.  
  tx->st = SSMPTCP_TX_DONE;
}
