#include <ssm_lht_ref.h>

void *ssm_lht_aafind(ssm_Iaa aa, void *key)
{
  ssm_dtrace(p, aa, p, key);
  ssm_lhtaa lhtaa = (ssm_lhtaa)aa;
  ssm_lhtaan node;
  ssm_lhtcmp_t cmp;
  cmp.len = lhtaa->keylen;
  cmp.key = key;
  node = tommy_hashlin_search(&(lhtaa->hashlin), ssm_lht_cmp, &cmp, ssm_lht_hash(key, lhtaa->keylen));

  return node->data;
}
