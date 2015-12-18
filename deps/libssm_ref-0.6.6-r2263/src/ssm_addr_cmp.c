#include <ssm_ref.h>

int ssm_addr_cmp(ssm_id id, ssm_Haddr a, ssm_Haddr b)
{
  return id->addr->cmp(id->addr, a, b);
}

