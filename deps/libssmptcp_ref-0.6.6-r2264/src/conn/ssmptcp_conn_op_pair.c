#include <ssmptcp_ref.h>

int ssmptcp_conn_op_pair(ssmptcp_conn conn, uint64_t a, uint64_t b)
{
  ssm_dtrace(p, conn, d, a, d, b);
  void *head_buf = SSM_MALLOC(a);

  ssmptcp_conn_read(conn, head_buf, a);

  ssm_mr head = ssm_mr_create(NULL, head_buf, a);
  
  ssm_mmr_t mmr;
  mmr.mr = head;
  mmr.map = NULL;
  ssm_tpr tpr = NULL;

  ssm_mmr dest;
  ssm_mr dest_mr;
  ssm_Il heads = ssm_ll_l(SSM_NOF);
  heads->addtop(heads, &mmr);
  ssm_Il tprs = ssm_ll_l(SSM_NOF);

  // ask SSM
  conn->tp->host->post(conn->tp->host, &(conn->addr), tprs, heads);
  tpr = tprs->remtop(tprs);
  tprs->del(tprs);
  heads->del(heads);

  dest = tpr->mmr;
  if(dest)  // match!
    dest_mr = dest->mr;
  else
    dest_mr = NULL;
  ssmptcp_tx tx = SSMPTCP_NEW(tx);
  ssmptcp_tx_init(tx, (conn->next_id)++, b, dest_mr, NULL, SSMPTCP_CALL_REMOTE, tpr->reply, tpr->tx);
  conn->txs->addbot(conn->txs, tx);
  ssm_mr_destroy(head);
  SSM_DEL(tpr);
  SSM_DEL(head_buf);
}
