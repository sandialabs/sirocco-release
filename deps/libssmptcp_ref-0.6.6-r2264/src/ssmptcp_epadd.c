#include <ssmptcp_ref.h>

void ssmptcp_epadd(ssmptcp_tp tp, ssmptcp_conn conn)
{
  ssm_dtrace(p, tp, p, conn);
  int r = epoll_ctl(tp->epfd, EPOLL_CTL_ADD, conn->fd, &(conn->epev));
  if(r == -1)
    ssm_eprint(lit, "Error trying EPOLL_CTL_ADD", s, strerror(errno));
}
