#include <ssmptcp_ref.h>

int ssmptcp_conn_writev(ssmptcp_conn conn, struct iovec *iovs, int len)
{
  while(len)
  {
    ssmptcp_conn_write(conn, iovs->iov_base, iovs->iov_len);
    len--;
    iovs++;
  }
  return 0;
}
