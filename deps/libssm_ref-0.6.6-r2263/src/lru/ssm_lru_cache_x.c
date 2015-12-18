#include <ssm_lru_ref.h>

ssm_Icache ssm_lru_cache_x(ssm_Iaa aa, ssm_Il l, int size, ssm_cb dropcb)
{
  ssm_lrucache lrucache = SSM_NEW(lrucache);

  lrucache->cache.put = ssm_lru_cacheput;
  lrucache->cache.find = ssm_lru_cachefind;
  lrucache->cache.rem = ssm_lru_cacherem;
  lrucache->cache.del = ssm_lru_cachedel;
  lrucache->cb = dropcb;
  lrucache->aa = aa;
  lrucache->l = l;
  lrucache->max = size;
  lrucache->size = 0;

  return (ssm_Icache)lrucache;
}
