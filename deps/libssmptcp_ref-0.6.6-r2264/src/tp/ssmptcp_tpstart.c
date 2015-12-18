#include <ssmptcp_ref.h>

int ssmptcp_tpstart(ssm_Itp itp, ssm_Ihost host, ssm_tpinfo reply, size_t headmaxlen)
{
  int r;

  ssm_dtrace(p, itp, p, host);
  ssmptcp_tp tp = (ssmptcp_tp) itp;
  tp->headmaxlen = headmaxlen;
  tp->active = 0;
  tp->host = host;
  
  // open listener

  struct sockaddr_in serveraddr;
  tp->connfd = socket(AF_INET, SOCK_STREAM, 0);
  if(tp->connfd < 0)
    ssm_eprint(lit, "socket() failed");
  bzero((char *)&serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = tp->port;
  serveraddr.sin_addr.s_addr = INADDR_ANY;
  int yes = 1;
  if(setsockopt(tp->connfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
  {
    ssm_eprint(lit, "couldn't setsockopt SO_REUSEADDR", s, strerror(errno));
  }
  if(setsockopt(tp->connfd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(int)) == -1)
  {
    ssm_eprint(lit, "couldn't setsockopt TCP_NODELAY", s, strerror(errno));
  }
  if(bind(tp->connfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
  {
    ssm_eprint(lit, "binding address failed: ", d, tp->port, s, strerror(errno));
  }
  listen(tp->connfd, 128);

  tp->epfd = epoll_create(SSMPTCP_MAXCONNS);
  tp->epevs = SSM_MALLOCV(sizeof(struct epoll_event), SSMPTCP_EVENTLEN);
  tp->epevlen = SSMPTCP_EVENTLEN;
  ssm_ddtrace(d, tp->epfd);

  // we have to actually listen on the listener
  tp->connfdev.events = EPOLLIN;
  tp->connfdev.data.ptr = &(tp->connfd);
  epoll_ctl(tp->epfd, EPOLL_CTL_ADD, tp->connfd, &(tp->connfdev));

  // set up a pipe to allow us to wake from a wait
  r = pipe(tp->ctlfd);
  if(r)
  {
    ssm_eprint(lit, "pipe returned error", s, strerror(errno));
  }
  tp->ctl_len = 0;
  tp->ctlfdev.events = EPOLLIN;
  tp->ctlfdev.data.ptr = tp->ctlfd;
  epoll_ctl(tp->epfd, EPOLL_CTL_ADD, tp->ctlfd[0], &(tp->ctlfdev));

  tp->dropcb.pcb = ssmptcp_dropcb;
  tp->dropcb.cbdata = tp;
  tp->conns = ssm_lock_cache(ssm_spinlock_lock(), ssm_lru_cache(SSMPTCP_SIZEOF(addr), SSMPTCP_MAXCONNS, &(tp->dropcb)));
  tp->fail_conns = ssm_lock_l(ssm_spinlock_lock(), ssm_ll_l(SSM_NOF));
  tp->cleanup_conns = ssm_lock_l(ssm_spinlock_lock(), ssm_ll_l(SSM_NOF));
  tp->txs = ssm_ll_l(SSM_NOF);
  return 0;
  
}
