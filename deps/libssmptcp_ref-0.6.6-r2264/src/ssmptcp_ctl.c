#include <ssmptcp_ref.h>

void ssmptcp_ctl(ssmptcp_tp tp)
{
  char c;
  int r;
  // we only ever use the ctl pipe to wake up, so waste the data
  while(tp->ctl_len)
  {
    r = read(tp->ctlfd[0], &c, 1);
    if(r < 0)
    {
      ssm_eprint(lit, "ctl write returned error", s, strerror(errno));
    }
    tp->ctl_len--; 
  }
}
