#include <ssm_lru_ref.h>

int ssm_lru_cacheput(ssm_Icache cache, void *key, void *val, int weight)
{
  ssm_lruentry entry;
  if(weight < 1)
  {
    ssm_wprint(lit, "weight is 0 or negative", d, weight);
  }
  ssm_lrucache lrucache = (ssm_lrucache) cache;
  entry = ssm_lru_cachefind(cache, key);
  if(entry)
  {
    ssm_wprint(lit, "entry already exists in cache", p, entry);
    return -1;
  }
  entry = SSM_NEW(lruentry);
  lrucache->l->addtop(lrucache->l, entry);
  entry->size = weight;
  entry->data = val;
  entry->key = key;
  lrucache->aa->ins(lrucache->aa, key, entry);
  lrucache->size += entry->size;
  while(lrucache->size > lrucache->max)
    ssm_lru_cachedrop(lrucache);
  return 0;
}
