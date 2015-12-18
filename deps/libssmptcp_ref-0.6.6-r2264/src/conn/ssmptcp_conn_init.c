#include <ssmptcp_ref.h>

ssmptcp_conn ssmptcp_conn_init(ssmptcp_tp tp, ssmptcp_addr addr)
{
  // open a connection to someone remotely
  ssm_dtrace(p, tp, s, addr->host, d, addr->port);
  struct sockaddr_in sockaddr_dest;
  ssmptcp_sockaddr(&sockaddr_dest, addr);
  
  ssmptcp_conn result = SSMPTCP_NEW(conn);
  memmove(&(result->addr), addr, SSMPTCP_SIZEOF(addr));
  result->tp = tp;
  result->txs = ssm_lock_l(ssm_spinlock_lock(), ssm_ll_l(SSM_NOF));
  result->next_id = 1;
  result->st = SSMPTCP_CONN_READY;
  result->err = SSMPTCP_ERR_OK;
  
  result->fd = socket(AF_INET, SOCK_STREAM, 0);
  int yes = 1;
  int r = connect(result->fd, (struct sockaddr *)&sockaddr_dest, sizeof(struct sockaddr));
  if(r < 0)
  {
    ssm_wprint(lit, "can't connect to addr", s, addr->host, d, addr->port, d, errno, s, strerror(errno));
    result->st = SSMPTCP_CONN_FAIL; 
    result->err = SSMPTCP_ERR_INVALID_ADDR;
    tp->fail_conns->addbot(tp->fail_conns, result);
  }
  else
    ssm_ddprint(lit, "connect returned", d, r);
  if(setsockopt(result->fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(int)) == -1)
  {
    ssm_eprint(lit, "couldn't setsockopt TCP_NODELAY", s, strerror(errno));
  }
  // set up epoll stuff
  result->epev.events = EPOLLIN;
  result->epev.data.ptr = result;
  ssmptcp_epadd(tp, result);

  // put in cache
  tp->active++;
  tp->conns->put(tp->conns, &(result->addr), result, 1);

  ssmptcp_conn_send(result, 0, SSMPTCP_OP_RPORT, NULL, NULL, tp->port, 0);
  ssm_dprint(lit, "new conn", p, result);
  return result;
}
