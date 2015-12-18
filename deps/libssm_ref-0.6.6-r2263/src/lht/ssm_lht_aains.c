#include <ssm_lht_ref.h>

int ssm_lht_aains(ssm_Iaa aa, void *key, void *value)
{
  ssm_lhtaa lhtaa = (ssm_lhtaa)aa;
  
  ssm_lhtaan node = SSM_NEW(lhtaan);
  node->data = value;
  tommy_hashlin_insert(&(lhtaa->hashlin), &(node->node), node, ssm_lht_hash(key, lhtaa->keylen));
  return 0;
  
}
