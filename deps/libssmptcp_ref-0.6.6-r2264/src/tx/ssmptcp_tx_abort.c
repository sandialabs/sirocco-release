#include <ssmptcp_ref.h>

int ssmptcp_tx_abort(ssmptcp_tx tx)
{
  if(tx->st != SSMPTCP_TX_DONE)
    tx->st = SSMPTCP_TX_FAIL;
  return 0;
}
