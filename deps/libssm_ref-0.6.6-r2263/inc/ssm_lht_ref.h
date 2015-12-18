#ifndef __SSM_LHT_REF_H
#define __SSM_LHT_REF_H

#include <ssm/lht.h>
#include <ssm/ext/tommyds/tommy.h>

SSM_TYPE(lhtaa)
{
  ssm_Iaa_t aa;
  tommy_hashlin hashlin;
  size_t keylen;
};

SSM_TYPE(lhtaan)
{
  tommy_hashlin_node node;
  void *data;
};

SSM_TYPE(lhtcmp)
{
  size_t len;
  void *key;
};

extern int ssm_lht_aains(ssm_Iaa aa, void *key, void *value);
extern void * ssm_lht_aarem(ssm_Iaa aa, void *key);
extern void * ssm_lht_aafind(ssm_Iaa aa, void *key);
extern int ssm_lht_aadel(ssm_Iaa aa);
extern int ssm_lht_cmp(void *a, void *b);

static inline uint32_t ssm_lht_hash(void *key, size_t len)
{
  return tommy_hash_u32(0xBAADF00D, key, len);
}
#endif
