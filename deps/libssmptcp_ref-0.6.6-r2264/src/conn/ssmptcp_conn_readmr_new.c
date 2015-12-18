#include <ssmptcp_ref.h>

ssm_mr ssmptcp_conn_readmr_new(ssmptcp_conn conn, size_t len)
{
  void *data = SSM_MALLOC(len);
  int r = ssmptcp_conn_read(conn, data, len);
  if(r)
  {
    SSM_DEL(data);
    return NULL;
  }
  ssm_mr result = ssm_mr_create(NULL, data, len);
  ssm_dprint(p, result);
  return result;
}
