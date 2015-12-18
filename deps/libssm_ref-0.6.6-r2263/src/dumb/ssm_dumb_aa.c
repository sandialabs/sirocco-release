#include <ssm_dumb_ref.h>

ssm_Iaa ssm_dumb_aa(size_t keylen)
{
  ssm_ddtrace(d, keylen);
  ssm_dumbaa result = SSM_NEW(dumbaa);
  result->aa.ins = ssm_dumb_aains;
  result->aa.rem = ssm_dumb_aarem;
  result->aa.find = ssm_dumb_aafind;
  result->aa.del = ssm_dumb_aadel;
  result->l = ssm_ll_l(SSM_NOF);
  result->keylen = keylen;
  ssm_ddprint(p, result, p, result->aa.ins, p, result->aa.rem, p, result->aa.find, p, result->aa.del, p, result->l);
  return (ssm_Iaa) result;
}
