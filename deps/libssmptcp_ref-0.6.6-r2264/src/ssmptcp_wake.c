#include <ssmptcp_ref.h>

int ssmptcp_wake(ssmptcp_tp tp)
{
  int r;
  tp->ctl_len++;
  char c = 'A';
  r = write(tp->ctlfd[1], &c, 1);
  if(r < 0)
  {
    ssm_eprint(lit, "ctl write returned error", s, strerror(errno));
  }
}
