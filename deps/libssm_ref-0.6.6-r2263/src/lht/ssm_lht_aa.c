#include <ssm_lht_ref.h>

ssm_Iaa ssm_lht_aa(size_t keylen)
{
  ssm_lhtaa result = SSM_NEW(lhtaa);

  result->keylen = keylen;
  result->aa.ins = ssm_lht_aains;
  result->aa.rem = ssm_lht_aarem;
  result->aa.find = ssm_lht_aafind;
  result->aa.del = ssm_lht_aadel;
  tommy_hashlin_init(&result->hashlin);
  return (ssm_Iaa)result;
}
