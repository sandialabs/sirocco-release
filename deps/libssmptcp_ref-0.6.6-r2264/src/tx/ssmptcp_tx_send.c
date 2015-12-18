#include <ssmptcp_ref.h>

int ssmptcp_tx_send(ssmptcp_tx tx, ssmptcp_conn conn)
{
  ssm_dtrace(p, tx, p, conn, s, ssmptcp_op_str(tx->next_op));

  uint64_t a, b;
  a = 0;
  b = 0;
  int free_mr = 0;
  int r;
  ssm_mr mr = NULL;
  ssm_md md = NULL;

  tx->st = SSMPTCP_TX_WAIT;

  switch(tx->next_op)
  {
    case SSMPTCP_OP_PAIR:
      if(tx->local_mr)
        tx->next_op = SSMPTCP_OP_ACK;
      else
        tx->next_op = SSMPTCP_OP_STOP;
      return ssmptcp_tx_send(tx, conn);
      break;
    case SSMPTCP_OP_MD: 
      a = tx->op_md_buf_len;
      b = 0;
      mr = ssm_mr_create(NULL, tx->op_md_buf, tx->op_md_buf_len);
      free_mr = 1;
      break;
    case SSMPTCP_OP_DATA: 
      a = ssm_mr_len(tx->local_mr);
      mr = tx->local_mr;
      if(tx->local_call = SSMPTCP_CALL_REMOTE)
        md = tx->op_md;
      break;
    case SSMPTCP_OP_REQ: 
      break;
    case SSMPTCP_OP_DONE:
      tx->st = SSMPTCP_TX_DONE;
      break;
    case SSMPTCP_OP_ACK:
      a = tx->id;
      b = tx->ssmreply;
      break;
    case SSMPTCP_OP_STOP:
      b = tx->ssmreply;
      break;
    default:
      ssm_wprint(lit, "Unsupported op", s, ssmptcp_op_str(tx->next_op), d, tx->next_op);
      tx->st = SSMPTCP_TX_FAIL; 
      return -1;
  }

  r = ssmptcp_conn_send(conn, tx->remote_id, tx->next_op, mr, md, a, b);
  if(free_mr)
    ssm_mr_destroy(mr);
  if(r)
  {
    tx->st = SSMPTCP_TX_FAIL;
    return -1;
  }
  else
  {
    return 0; 
  }
}
