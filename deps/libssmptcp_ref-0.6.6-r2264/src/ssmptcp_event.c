#include <ssmptcp_ref.h>

void ssmptcp_event(ssmptcp_tp tp, struct epoll_event *epev)
{
  ssm_dtrace(p, tp, p, epev, p, epev->data.ptr, d, epev->events);
  int r = 0;

  ssm_assert(epev->data.ptr, tp);
  // is event for connection port?
  if(epev->data.ptr == &(tp->connfd))
  {
    ssmptcp_accept(tp);
  }
  // is event for control port?
  else if(epev->data.ptr == tp->ctlfd)
  {
    ssmptcp_ctl(tp);
  }
  else
  {
  // event is conn type
  ssmptcp_conn conn = epev->data.ptr;
  if(epev->events & EPOLLIN)
    r += ssmptcp_conn_epollin(conn);
  if(epev->events & EPOLLOUT)
    r += ssmptcp_conn_epollout(conn);
  if(r > 0) // if someone put their greasy paws on the epoll event flags
    ssmptcp_epmod(tp, conn);
  }

}
