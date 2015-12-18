#include <ssmptcp_ref.h>

int ssmptcp_tx_init(ssmptcp_tx tx, uint32_t id, uint32_t remote_id, ssm_mr local_mr, ssm_md local_md, ssmptcp_call local_call, ssm_tpreply reply, ssm_Htx ssmtx)
{
  ssm_dtrace(p, tx, d, id, d, remote_id, p, local_mr, p, local_md, s, ssmptcp_call_str(local_call), s, ssm_tpreply_str(reply), p, ssmtx);
  tx->id = id;
  tx->remote_id = remote_id;
  if(local_call == SSMPTCP_CALL_REMOTE)
    tx->st = SSMPTCP_TX_SEND;
  else
    tx->st = SSMPTCP_TX_WAIT;
  tx->local_mr = local_mr;
  tx->local_md = local_md;
  tx->local_call = local_call;

  tx->op_md = NULL;
  tx->op_md_buf = NULL;
  tx->op_md_buf_len = 0;
  tx->remote_call = SSMPTCP_CALL_UNKNOWN;
  tx->next_op = SSMPTCP_OP_PAIR;

  tx->ssmreply = reply;
  tx->ssmtx = ssmtx;

  return 0;
}
