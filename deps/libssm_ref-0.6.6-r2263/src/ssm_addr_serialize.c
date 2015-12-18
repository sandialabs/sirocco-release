#include <ssm_ref.h>

size_t ssm_addr_serialize(char *out, size_t len, ssm_id id, ssm_Haddr addr)
{
  return id->addr->serialize(id->addr, out, len, addr);
}
