#include <ssm_mr_ref.h>

int ssm_mr_destroy(ssm_mr mr)
{
  SSM_DEL(mr);
  return 0;
}
