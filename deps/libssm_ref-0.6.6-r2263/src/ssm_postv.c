#include <ssm_ref.h>

int ssm_postv(ssm_id id, ssm_me me, struct iovec *iovs, int len, ssm_Fop flags)
{
  return ssm_post(id, me, ssm_iovs_mr(iovs, len), flags);
}
