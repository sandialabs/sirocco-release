#include <ssm_mr_ref.h>

extern size_t ssm_mr_cpmd(ssm_mr target, ssm_md targetmd, ssm_mr src, ssm_md srcmd)
{
  size_t tvl = ssm_mr_mdiov(NULL, target, targetmd, 0);
  size_t svl = ssm_mr_mdiov(NULL, src, srcmd, 0);
  struct iovec *tv = SSM_TMALLOCV(struct iovec, tvl);
  struct iovec *sv = SSM_TMALLOCV(struct iovec, svl);
  ssm_mr_mdiov(tv, target, targetmd, tvl);
  ssm_mr_mdiov(sv, src, srcmd, svl);
  ssm_iovcopy(tv, tvl, sv, svl);
  SSM_FREE(tv);
  SSM_FREE(sv);
  return 0;
}
