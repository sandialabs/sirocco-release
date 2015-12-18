#include <ssm_lru_ref.h>

int ssm_lru_cachedel(ssm_Icache cache)
{
  ssm_lrucache lrucache = (ssm_lrucache) cache;
  while(lrucache->l->top(lrucache->l) != NULL)
    ssm_lru_cachedrop(lrucache);
  lrucache->aa->del(lrucache->aa);
  lrucache->l->del(lrucache->l);
  if(lrucache->size != 0)
    ssm_wprint(lit, "final size is nonzero", d, lrucache->size);
  SSM_DEL(lrucache);
  return 0;
}
