#include <ssmptcp_ref.h>

int ssmptcp_conn_send(ssmptcp_conn conn, uint32_t remote_id, ssmptcp_op op, ssm_mr mr, ssm_md md, uint64_t a, uint64_t b)
{
  ssm_dtrace(p, conn, d, remote_id, s, ssmptcp_op_str(op), p, mr, p, md, d, a, d, b);
  ssmptcp_hdr_t hdr;
  hdr.op = op;
  hdr.a = a;
  hdr.b = b;
  hdr.id = remote_id;

  ssmptcp_conn_write(conn, &hdr, SSMPTCP_SIZEOF(hdr));
  if(mr)
    ssmptcp_conn_write_mr(conn, mr, md);
}
