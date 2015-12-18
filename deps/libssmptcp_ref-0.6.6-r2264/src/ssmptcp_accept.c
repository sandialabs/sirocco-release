#include <ssmptcp_ref.h>

void ssmptcp_accept(ssmptcp_tp tp)
{
  ssm_dtrace(p, tp);
  // connection port event told us there's a connection to accept
  int newfd;
  struct sockaddr_in newaddr;
  socklen_t socklen = sizeof(struct sockaddr);
  newfd = accept(tp->connfd, (struct sockaddr *)&newaddr, &socklen);
  int yes = 1;
  if(newfd < 0)
  {
    ssm_eprint(lit, "error on accept", s, strerror(errno));
    return;
  }
  if(setsockopt(newfd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(int)) == -1)
  {
    ssm_eprint(lit, "couldn't setsockopt TCP_NODELAY", s, strerror(errno));
  }
    
  // we're good; set up a new conn
  ssmptcp_conn conn = SSMPTCP_NEW(conn);
  bzero(conn, SSMPTCP_SIZEOF(conn));
  const char *cr = inet_ntop(AF_INET, &(newaddr.sin_addr), conn->addr.host, 20);
  if(cr == NULL)
        ssm_eprint(lit, "error on inet_ntop", s, strerror(errno) );
  conn->addr.port = 65535;
  conn->addr.origin = SSMPTCP_ADDR_REMOTE;
  ssm_dprint(lit, "Retrieved remote address", s, conn->addr.host, d, conn->addr.port);
  conn->tp = tp;
  conn->txs = ssm_lock_l(ssm_spinlock_lock(), ssm_ll_l(SSM_NOF));
  conn->next_id = 1;
  conn->st = SSMPTCP_CONN_READY;
  conn->err = SSMPTCP_ERR_OK;
  conn->epev.data.ptr = conn;
  conn->epev.events = EPOLLIN;
  conn->st = SSMPTCP_CONN_READY;
  conn->fd = newfd;
  ssmptcp_epadd(tp, conn);
  // we don't put in cache until we get RPORT
  // possible memory leak
}
