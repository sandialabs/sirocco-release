#include <ssmptcp_ref.h>

int ssmptcp_tx_op_md(ssmptcp_tx tx, ssmptcp_conn conn, uint64_t a, uint64_t b)
{
  // md incoming
  // no response needed, just read
  // a = md serialized len, b = len of data described
  int r;
  tx->op_md_buf = SSM_MALLOC(a);
  r = ssmptcp_conn_read(conn, tx->op_md_buf, a);
  if(r)
  {
    SSM_DEL(tx->op_md_buf);
    tx->st = SSMPTCP_TX_FAIL;
    return -1;
  }
  tx->op_md = ssm_md_load(tx->op_md_buf, a);
  return 0;
}
