#ifndef __SSM_LRU_H
#define __SSM_LRU_H

#include <ssm/if.h>
#include <ssm/cb.h>
#include <ssm/ll.h>
#include <ssm/lht.h>
#include <ssm/dumb.h>

extern ssm_Icache ssm_lru_cache_x(ssm_Iaa aa, ssm_Il l, int size, ssm_cb dropcb);

static inline ssm_Icache ssm_lru_cache(size_t keylen, int size, ssm_cb dropcb)
{
  return ssm_lru_cache_x(ssm_dumb_aa(keylen), ssm_ll_l(SSM_NOF), size, dropcb);
}

#endif
