#include <ssmptcp_ref.h>

void ssmptcp_tx_kill(ssmptcp_tx tx)
{
  ssm_dtrace(p, tx);
  if(tx->op_md_buf)
  {
    SSM_DEL(tx->op_md_buf);
  }
  if(tx->op_md)
  {
    ssm_md_release(tx->op_md);
  }
  SSM_DEL(tx);
}
