#include <ssm_ref.h>

ssm_Haddr ssm_addr_local(ssm_id id)
{
  return id->addr->local(id->addr);
}

