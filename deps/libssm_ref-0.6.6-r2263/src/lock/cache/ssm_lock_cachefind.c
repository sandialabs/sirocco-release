#include <ssm_lock_ref.h>

void * ssm_lock_cachefind(ssm_Icache cache, void *key)
{
  void * result;
  ssm_lockcache lockcache = (ssm_lockcache)cache;
  
  lockcache->lock->share(lockcache->lock);
  result = lockcache->base->find(lockcache->base, key);
  lockcache->lock->rel(lockcache->lock);

  return result;
}

