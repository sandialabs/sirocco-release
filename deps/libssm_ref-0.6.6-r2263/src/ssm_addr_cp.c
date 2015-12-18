#include <ssm_ref.h>

ssm_Haddr ssm_addr_cp(ssm_id id, ssm_Haddr src)
{
  return id->addr->cp(id->addr, src);
}

