#include <ssm_ref.h>

ssm_Haddr ssm_addr_create(ssm_id id, ssm_Haddrargs opts)
{
  return id->addr->create(id->addr, opts);
}

