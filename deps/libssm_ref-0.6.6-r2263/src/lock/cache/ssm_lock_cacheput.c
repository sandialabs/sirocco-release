#include <ssm_lock_ref.h>

int ssm_lock_cacheput(ssm_Icache cache, void *key, void *value, int size)
{
  int result;
  ssm_lockcache lockcache = (ssm_lockcache)cache;
  
  lockcache->lock->share(lockcache->lock);
  result = lockcache->base->put(lockcache->base, key, value, size);
  lockcache->lock->rel(lockcache->lock);

  return result;
}


