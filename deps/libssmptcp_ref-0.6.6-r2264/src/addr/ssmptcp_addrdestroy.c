#include <ssmptcp_ref.h>

int ssmptcp_addrdestroy(ssm_Iaddr addr, ssm_Haddr target)
{
  SSM_DEL(target);
  return 0;
}

