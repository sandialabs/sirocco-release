#include <ssm_ref.h>

ssm_Iaddr ssm_addr(ssm_id id)
{
  ssm_passert(id);
  return id->tp->addr(id->tp);
}
