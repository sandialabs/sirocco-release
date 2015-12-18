#include <ssmptcp_ref.h>

int ssmptcp_addrcmp(ssm_Iaddr addr, ssm_Haddr a, ssm_Haddr b)
{
  return memcmp(a, b, SSMPTCP_SIZEOF(addr));
}
