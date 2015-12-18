#include <ssm_btree_ref.h>

void *ssm_btree_aarem(ssm_Iaa aa, void *key)
{
  ssm_btree btree = (ssm_btree) aa;
  ssm_btree_node *node = ssm_btree_node_find(btree->cmp, btree->cmparg, &(btree->head), key);
  ssm_btree_node n = *node;
  void *v;
  if(n == NULL)
    return NULL;
  v = n->value;
  if(!(n->left) && !(n->right)) // easy, no children
  {
    *node = NULL;
  }
  else if(!(n->left)) // right child, easy
  {
    *node = n->right;
  }
  else if(!(n->right)) // left child, easy
  {
    *node = n->left;
  }
  else // hard, two children
  {
    // descend to find smallest in right tree and swap it with this node
    ssm_btree_node *leftmost = ssm_btree_node_leftmost(&(n->right));
    *node = *leftmost;
    *leftmost = NULL;
  }
  SSM_DELETE(n);
  return v;
}
