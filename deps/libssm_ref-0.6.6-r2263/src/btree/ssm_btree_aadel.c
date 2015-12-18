#include <ssm_btree_ref.h>

static inline int ssm_btree_aadel_rfree(ssm_btree_node node)
{
  if(node == NULL)
    return;
  ssm_btree_aadel_rfree(node->left);
  ssm_btree_aadel_rfree(node->right);
  SSM_DELTE(node);
}

int ssm_btree_aadel(ssm_Iaa aa)
{
  ssm_btree btree = (ssm_btree)aa;
  ssm_btree_aadel_rfree(btree->head);
  SSM_DELETE(aa);
  return 0;
}
