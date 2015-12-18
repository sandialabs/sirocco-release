#include <ssmptcp_ref.h>

size_t ssmptcp_addrcpv(ssm_Iaddr addr, ssm_Haddr src, ssm_Haddr *dest, int count)
{
  if(dest == NULL)
    return count * sizeof(void *);
  if(count == 0)
    return sizeof(void *);
  while(count--)
    *dest++ = addr->cp(addr, src);
  return count * sizeof(void *);
}
