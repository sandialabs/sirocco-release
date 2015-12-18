#include <ssmptcp_ref.h>

int ssmptcp_tpwake(ssm_Itp itp)
{
  ssmptcp_tp tp = (ssmptcp_tp) itp;
  ssmptcp_wake(tp);
}
