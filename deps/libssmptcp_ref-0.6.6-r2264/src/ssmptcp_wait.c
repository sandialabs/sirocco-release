#include <ssmptcp_ref.h>

int ssmptcp_tpwait(ssm_Itp itp, long usec)
{
  int delay = (usec < 1) ? -1 : usec / 1000;
  ssm_dtrace(p, itp, d, usec);
  ssmptcp_tp tp = (ssmptcp_tp) itp;
  // clean out failed connections
  if(tp->fail_conns->len(tp->fail_conns))
  {
    while(tp->fail_conns->len(tp->fail_conns))
    {
      ssmptcp_conn conn = tp->fail_conns->remtop(tp->fail_conns);
      ssm_dprint(lit, "Dropping fail_conn", p, (tp->fail_conns));
      tp->conns->rem(tp->conns, &(conn->addr));
    }
    return 0;
  }
  // free the cleanup list
  while(tp->cleanup_conns->len(tp->cleanup_conns))
  {
    ssmptcp_conn conn = tp->cleanup_conns->remtop(tp->cleanup_conns);
    SSM_DEL(conn);
  }
  // epoll block on our epoll fd, then handle events
  ssm_ddprint(d, tp->epfd, p, tp->epevs, d, tp->epevlen, d, delay);
  int evs = epoll_wait(tp->epfd, tp->epevs, tp->epevlen, delay);
  ssm_dprint(lit, "epoll_wait returned", d, evs);
  if(evs == -1)
    ssm_eprint(lit, "epoll_wait error", s, strerror(errno));
  int r = evs;
  struct epoll_event *epev = tp->epevs;
  while(evs > 0)
  {
    ssmptcp_event(tp, epev);
    evs--;
    epev++;
  }
  return r;
}
