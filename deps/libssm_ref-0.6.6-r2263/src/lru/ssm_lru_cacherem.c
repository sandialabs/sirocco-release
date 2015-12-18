#include <ssm_lru_ref.h>

void *ssm_lru_cacherem(ssm_Icache cache, void *key)
{
  ssm_lrucache lrucache = (ssm_lrucache) cache;
  ssm_lruentry result = lrucache->aa->rem(lrucache->aa, key);
  void *rdata = NULL;
  if(result)
  {
    rdata = result->data;
    ssm_cb_invoke(lrucache->cb, result->data);
    lrucache->l->rem(lrucache->l, result);
    lrucache->size -= result->size;
    SSM_DELETE(result);
  }
  return rdata;
}
