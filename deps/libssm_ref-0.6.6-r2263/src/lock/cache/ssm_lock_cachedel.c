#include <ssm_lock_ref.h>

int ssm_lock_cachedel(ssm_Icache cache)
{
  int result;
  ssm_lockcache lockcache = (ssm_lockcache)cache;
  
  lockcache->lock->arm(lockcache->lock);
  result = lockcache->base->del(lockcache->base);
  lockcache->lock->rel(lockcache->lock);

  lockcache->lock->del(lockcache->lock);
  SSM_DELETE(lockcache);
  return result;
}
