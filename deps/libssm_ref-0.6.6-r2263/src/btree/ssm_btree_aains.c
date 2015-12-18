#include <ssm_btree_ref.h>

int ssm_btree_aains(ssm_Iaa aa, void *key, void *value)
{
  ssm_btree btree = (ssm_btree)aa;
  ssm_btree_node *target = ssm_btree_node_find(btree->cmp, btree->cmparg, &(btree->head), key);
  if(*target != NULL)
    return 1;
  *target = ssm_btree_node_new(key, value);
  return 0;
}

