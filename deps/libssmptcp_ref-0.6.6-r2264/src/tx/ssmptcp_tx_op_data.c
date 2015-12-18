#include <ssmptcp_ref.h>

int ssmptcp_tx_op_data(ssmptcp_tx tx, ssmptcp_conn conn, uint64_t a, uint64_t b)
{
  // data!  
  // a = len, b = sends remaining
  // some ops don't send an MD (e.g. pull) so just use a sensible default
  if(!(tx->op_md))
    tx->op_md = ssm_md_add(NULL, 0, a);
  
  ssmptcp_conn_readmr(conn, tx->local_mr, tx->op_md);

  if(!b)
  {
    tx->st = SSMPTCP_TX_SEND;
    tx->next_op = SSMPTCP_OP_DONE; 
  }  
  return 0;

}
