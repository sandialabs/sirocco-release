#include <ssm_ref.h>

int ssm_poll(ssm_id id)
{
  ssm_update(id);
  return ssm_doevent(id);
}
