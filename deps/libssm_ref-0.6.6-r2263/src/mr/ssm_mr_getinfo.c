#include <ssm_mr_ref.h>

int ssm_mr_getinfo(ssm_mrinfo info, ssm_mr mr)
{
  memmove(info, &(mr->info), SSM_SIZEOF(mrinfo));
  return 0;
}
