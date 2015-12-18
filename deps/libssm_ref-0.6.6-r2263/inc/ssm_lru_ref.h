#ifndef __SSM_LRU_REF_H
#define __SSM_LRU_REF_H

#include <ssm/pp.h>
#include <ssm/if.h>
#include <ssm/cb.h>

SSM_TYPE(lrucache)
{
  ssm_Icache_t cache;
  ssm_Iaa aa;
  ssm_Il  l;
  ssm_cb  cb;
  int max;
  int size;
};

SSM_TYPE(lruentry)
{
  void *data;
  void *key;
  int  size;
};

extern int ssm_lru_cacheput(ssm_Icache cache, void *key, void *value, int size);
extern void* ssm_lru_cachefind(ssm_Icache cache, void *key);
extern void* ssm_lru_cacherem(ssm_Icache cache, void *key);
extern int ssm_lru_cachedel(ssm_Icache cache);
extern int ssm_lru_cachedrop(ssm_lrucache lrucache);

#endif
