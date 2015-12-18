#include <ssmptcp_ref.h>

int ssmptcp_tpdel(ssm_Itp tp)
{
  SSM_DEL(tp);
  return 0;
}

