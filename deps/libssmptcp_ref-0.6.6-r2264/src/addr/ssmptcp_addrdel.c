#include <ssmptcp_ref.h>

int ssmptcp_addrdel(ssm_Iaddr addr)
{
  SSM_DEL(addr);
  return 0;
}
