#ifndef SSM_CACHE_H
#define SSM_CACHE_H

#include <ssm/ssm_pp.h>

SSM_INTERFACE_DECL(cache);

SSM_PROC(int, cacheput)(ssm_Icache cache, void *key, void *value, int weight);
SSM_PROC(void *, cachefind)(ssm_Icache cache, void *key);
SSM_PROC(void *, cacherem)(ssm_Icache cache, void *key);
SSM_PROC(int, cachedel)(ssm_Icache cache);

SSM_INTERFACE_DEFN(cache)
{
  ssm_Pcacheput put;
  ssm_Pcachefind find;
  ssm_Pcacherem rem;
  ssm_Pcachedel del;
};

#endif
