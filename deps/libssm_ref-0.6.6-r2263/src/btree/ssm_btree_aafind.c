#include <ssm_btree_ref.h>

void *ssm_btree_aafind(ssm_Iaa aa, void *key)
{
  ssm_btree btree = (ssm_btree) aa;
  ssm_btree_node *result = ssm_btree_node_find(btree->cmp, btree->cmparg, &(btree->head), key);
  if(*result == NULL)
    return NULL;
  return (*result)->value;
}
