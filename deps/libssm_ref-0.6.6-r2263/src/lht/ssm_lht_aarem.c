#include <ssm_lht_ref.h>

void *ssm_lht_aarem(ssm_Iaa aa, void *key)
{
  ssm_lhtaa lhtaa = (ssm_lhtaa)aa;
  ssm_lhtaan node;
  void *result;

  ssm_lhtcmp_t cmp;
  cmp.len = lhtaa->keylen;
  cmp.key = key;
  node = tommy_hashlin_remove(&(lhtaa->hashlin), ssm_lht_cmp, &cmp, ssm_lht_hash(key, lhtaa->keylen));
  result = node->data;
  SSM_DELETE(node);
  return result;
}
