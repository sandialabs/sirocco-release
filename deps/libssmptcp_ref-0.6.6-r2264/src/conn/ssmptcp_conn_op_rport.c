#include <ssmptcp_ref.h>

int ssmptcp_conn_op_rport(ssmptcp_conn conn, uint64_t a, uint64_t b)
{
  ssm_dtrace(p, conn, d, a, d, b);
  conn->addr.port = a;
  conn->tp->active++;
  conn->tp->conns->put(conn->tp->conns, &(conn->addr), conn, 1);
  return 0;
}
