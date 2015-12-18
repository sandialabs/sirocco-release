#include <ssmptcp_ref.h>

int ssmptcp_conn_write(ssmptcp_conn conn, void *p, size_t len)
{
  ssm_dtrace(p, conn, p, p, d, len);
  if(conn->st == SSMPTCP_CONN_FAIL)
    return -1;
  int r;
  while(len)
  {
    r = write(conn->fd, p, len);
    if(r == -1)
    {
      ssm_wprint(lit, "couldn't write");
      conn->st = SSMPTCP_CONN_FAIL;
      conn->err = SSMPTCP_ERR_WRITE_FAIL;
      conn->tp->fail_conns->addbot(conn->tp->fail_conns, conn);
      return -1;
    }
    len -= r;
    p += r;
  }
  return 0;
}
