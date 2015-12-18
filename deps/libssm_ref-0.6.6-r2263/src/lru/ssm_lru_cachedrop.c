#include <ssm_lru_ref.h>

int ssm_lru_cachedrop(ssm_lrucache lrucache)
{
  ssm_lruentry entry = lrucache->l->rembot(lrucache->l);
  entry = lrucache->aa->rem(lrucache->aa, entry->key);
  if(!entry)
  {
    ssm_eprint(lit, "Failed to find entry from LRU list!", p, entry, d, lrucache->size);
    return -1;
  }
  ssm_cb_invoke(lrucache->cb, entry->data);
  lrucache->size -= entry->size;
  SSM_DEL(entry);
  return 0;
}
