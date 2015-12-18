#include <ssmptcp_ref.h>

int ssmptcp_tx_ready(ssmptcp_tx tx)
{
  if(tx->st == SSMPTCP_TX_SEND)
    return 1;
  return 0;
}
