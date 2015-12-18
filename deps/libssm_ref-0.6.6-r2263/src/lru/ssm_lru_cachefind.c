#include <ssm_lru_ref.h>

void *ssm_lru_cachefind(ssm_Icache cache, void *key)
{
  int r;
  ssm_lrucache lrucache = (ssm_lrucache) cache;
  ssm_lruentry entry = lrucache->aa->find(lrucache->aa, key);
  if(entry)
  {
    r = lrucache->l->rem(lrucache->l, entry);
    if(!r)
    {
      ssm_wprint(lit, "Entry not found in LRU list!", p, entry, d, lrucache->l->len(lrucache->l), d, lrucache->size);
    }
    lrucache->l->addtop(lrucache->l, entry);
    return entry->data;
  }
  ssm_ddprint(lit, "returning NULL");
  return NULL;
}
