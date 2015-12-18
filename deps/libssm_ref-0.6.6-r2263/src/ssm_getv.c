#include <ssm_ref.h>

ssm_tx ssm_getv(ssm_id id, ssm_Haddr addr, struct iovec *iovs, int len, ssm_bits bits, ssm_cb cb, ssm_Fop flags)
{
  return ssm_get(id, addr, NULL, ssm_iovs_mr(iovs, len), bits, cb, flags);
}

