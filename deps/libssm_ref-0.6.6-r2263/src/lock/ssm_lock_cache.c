#include <ssm_lock_ref.h>

ssm_Icache ssm_lock_cache(ssm_Ilock lock, ssm_Icache cache)
{
  ssm_lockcache lockcache = SSM_NEW(lockcache);

  lockcache->cache.put = ssm_lock_cacheput;
  lockcache->cache.find = ssm_lock_cachefind;
  lockcache->cache.rem = ssm_lock_cacherem;
  lockcache->cache.del = ssm_lock_cachedel;
  lockcache->base = cache;
  lockcache->lock = lock;

  return (ssm_Icache)lockcache;
}



