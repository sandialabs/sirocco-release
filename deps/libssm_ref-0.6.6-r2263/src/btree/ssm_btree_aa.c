#include <ssm_btree_ref.h>

ssm_Iaa ssm_btree_aa(ssm_Pbtreecmp cmp, void *cmparg)
{
  ssm_btree result = SSM_NEW(btree);
  result->aa.ins = ssm_btree_aains;
  result->aa.rem = ssm_btree_aarem;
  result->aa.find = ssm_btree_aafind;
  result->aa.del = ssm_btree_aadel;
  result->head = NULL;
  result->cmp = cmp;
  result->cmparg = cmparg;
  return (ssm_Iaa)result;
}
