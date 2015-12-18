#include <ssm_ref.h>

ssm_tx ssm_putv(ssm_id id, ssm_Haddr addr, struct iovec *iovs, int len, ssm_bits bits, ssm_cb cb, ssm_Fop flags)
{
  return ssm_put(id, addr, ssm_iovs_mr(iovs, len), NULL, bits, cb, flags);
}
