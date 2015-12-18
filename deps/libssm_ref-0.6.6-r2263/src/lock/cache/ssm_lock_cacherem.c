#include <ssm_lock_ref.h>

void * ssm_lock_cacherem(ssm_Icache cache, void *key)
{
  void * result;
  ssm_lockcache lockcache = (ssm_lockcache)cache;
  
  lockcache->lock->arm(lockcache->lock);
  result = lockcache->base->rem(lockcache->base, key);
  lockcache->lock->rel(lockcache->lock);

  return result;
}


