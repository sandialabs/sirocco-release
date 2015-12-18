#include <ssmptcp_ref.h>

int ssmptcp_conn_epollin(ssmptcp_conn conn)
{
  // epoll says it's go time, son
  // since we don't let go to block until we're done with the hdr + any data,
  // we know that what is coming in is a hdr and can plan accordingly

  ssmptcp_hdr_t hdr;
  ssmptcp_tx tx;
  if(conn->tp == NULL)
  {
    return -10;
  }
  int r = ssmptcp_conn_read(conn, &hdr, SSMPTCP_SIZEOF(hdr));

  if(r != 0)
  {
    ssm_dprint(lit, "error return from ssmptcp_conn_read", d, r);
    return -10;
  }
  ssm_dprint(lit, "read hdr", s, ssmptcp_op_str(hdr.op));

  ssmptcp_conn_op(conn, hdr.op, hdr.id, hdr.a, hdr.b);

  ssmptcp_conn_finish(conn);

  if(ssmptcp_conn_senders(conn))
  {
    conn->epev.events |= EPOLLOUT;
    return 1;
  }
  // we don't ever remove the epollin bit, because we may always need to read
  return 0;
}
