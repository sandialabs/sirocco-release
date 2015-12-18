#include <ssmptcp_ref.h>

int ssmptcp_conn_write_mr(ssmptcp_conn conn, ssm_mr mr, ssm_md md)
{
  struct iovec *iovs;
  int len = ssm_mr_mdiov(NULL, mr, md, 0);
  iovs = SSM_TMALLOCV(struct iovec, len);
  ssm_mr_mdiov(iovs, mr, md, len);
  int r = ssmptcp_conn_writev(conn, iovs, len);
  SSM_DEL(iovs);
  return r;
}
