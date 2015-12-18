#include <ssm_lht_ref.h>

int ssm_lht_aadel(ssm_Iaa aa)
{
  ssm_lhtaa lhtaa = (ssm_lhtaa)aa;
  tommy_hashlin_done(&(lhtaa->hashlin));
  return 0;
}
