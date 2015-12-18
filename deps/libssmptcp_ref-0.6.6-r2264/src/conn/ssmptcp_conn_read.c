#include <ssmptcp_ref.h>

int ssmptcp_conn_read(ssmptcp_conn conn, void *p, size_t len)
{
  ssm_dtrace(p, conn, d, conn->fd, p, p, d, len);
  ssm_assert(conn, conn->tp);
  int r;
  while(len)
  {
    r = read(conn->fd, p, len);
    if(r == -1)
    {
      ssm_wprint(lit, "couldn't read");
      conn->st = SSMPTCP_CONN_FAIL;
      conn->err = SSMPTCP_ERR_READ_FAIL;
      conn->tp->fail_conns->addbot(conn->tp->fail_conns, conn);
      return -1;
    }
    if(r == 0)
    {
      ssm_dprint(lit, "0 bytes, assuming conn dead");
      conn->st = SSMPTCP_CONN_FAIL;
      conn->err = SSMPTCP_ERR_READ_FAIL;
      conn->tp->fail_conns->addbot(conn->tp->fail_conns, conn);
      return -1;
    }
    len -= r;
    p += r;
  }
  return 0;
}
