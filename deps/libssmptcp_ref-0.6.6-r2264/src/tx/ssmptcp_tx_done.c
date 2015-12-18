#include <ssmptcp_ref.h>

int ssmptcp_tx_done(ssmptcp_tx tx)
{
  if(tx->st == SSMPTCP_TX_DONE)
    return 1;
  if(tx->st == SSMPTCP_TX_FAIL)
    return 1;
  return 0;
}
