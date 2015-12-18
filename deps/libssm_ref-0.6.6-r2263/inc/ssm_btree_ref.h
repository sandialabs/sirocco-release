#ifndef __SSM_BTREE_REF_H
#define __SSM_BTREE_REF_H

#include <ssm/btree.h>

SSM_TYPE(btree_node)
{
  void *key;
  void *value;
  ssm_btree_node left;
  ssm_btree_node right;
};

SSM_TYPE(btree)
{
  ssm_Iaa_t aa;
  ssm_btree_node head;
  ssm_Pbtreecmp cmp;
  void * cmparg;
};

static inline ssm_btree_node ssm_btree_node_new(void *key, void *value)
{
  ssm_btree_node result = SSM_NEW(btree_node);
  result->key = key;
  result->value = value;
  result->left = NULL;
  result->right = NULL;
  return result;
}

static inline ssm_btree_node *ssm_btree_node_find(ssm_Pbtreecmp cmp, void *arg, ssm_btree_node *node, void *key)
{
  if(*node == NULL)
    return node;

  int result = cmp(arg, key, (*node)->key);
  if(result == 0)
    return node;
  if(result < 0)
    return ssm_btree_node_find(cmp, arg, &((*node)->left), key);
  return ssm_btree_node_find(cmp, arg, &((*node)->right), key);
}

static inline ssm_btree_node *ssm_btree_node_leftmost(ssm_btree_node *node)
{
  while((*node)->left)
    node = &((*node)->left);
  return node;
}
extern int ssm_btree_aains(ssm_Iaa aa, void *key, void *value);
extern void * ssm_btree_aarem(ssm_Iaa aa, void *key);
extern void * ssm_btree_aafind(ssm_Iaa aa, void *key);
extern int ssm_btree_aadel(ssm_Iaa aa);

#endif
