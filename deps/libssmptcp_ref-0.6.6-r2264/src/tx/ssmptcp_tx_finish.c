#include <ssmptcp_ref.h>

int ssmptcp_tx_finish(ssmptcp_tx tx, ssm_Ihost host)
{
  ssm_dtrace(p, tx, p, host, d, tx->id, s, ssmptcp_tx_st_str(tx->st), s, ssm_tpreply_str(tx->ssmreply), p, tx->ssmtx);
  if(tx->ssmtx == NULL) // no responsibilities
    return 0;

  if(tx->st == SSMPTCP_TX_FAIL)
    host->finish(host, tx->ssmtx, tx->op_md, tx->err, SSM_TPACT_FAILED);
  else if(tx->local_call = SSMPTCP_CALL_PUSH)
    host->finish(host, tx->ssmtx, tx->op_md, tx->ssmreply, SSM_TPACT_READ);
  else
    host->finish(host, tx->ssmtx, tx->op_md, tx->ssmreply, SSM_TPACT_WROTE);

  tx->st == SSMPTCP_TX_DEAD;
  return 0;
}
