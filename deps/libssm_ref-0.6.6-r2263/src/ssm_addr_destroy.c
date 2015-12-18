#include <ssm_ref.h>

int ssm_addr_destroy(ssm_id id, ssm_Haddr a)
{
  return id->addr->destroy(id->addr, a);
}

