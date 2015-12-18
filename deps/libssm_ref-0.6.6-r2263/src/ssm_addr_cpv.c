#include <ssm_ref.h>

size_t ssm_addr_cpv(ssm_id id, ssm_Haddr src, ssm_Haddr *dest, int count)
{
  return id->addr->cpv(id->addr, src, dest, count);
}

