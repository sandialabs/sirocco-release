#include <ssm_ref.h>

ssm_Haddr ssm_addr_load(ssm_id id, char *in, size_t len)
{
  return id->addr->load(id->addr, in, len);
}
