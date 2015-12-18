#ifndef SSM_LOCK_REF_H
#define SSM_LOCK_REF_H


#include <pthread.h>
#include <ssm/if/lock.h>

#if 0
struct _ssm_lock
{
  union
  {
    pthread_mutex_t mutex;
    pthread_spinlock_t spin;
    pthread_rwlock_t rw;
  } locks;
  ssm_Flock flags;
};
#endif

SSM_TYPE(locki)
{
  ssm_Ii_t i;
  ssm_Ii   base;
  ssm_Ilock lock;
};
extern void * ssm_lock_inext(ssm_Ii i);
extern void * ssm_lock_icur(ssm_Ii i);
extern int ssm_lock_imore(ssm_Ii i);
extern int ssm_lock_idel(ssm_Ii i);

SSM_TYPE(lockaa)
{
  ssm_Iaa_t aa;
  ssm_Iaa base;
  ssm_Ilock lock;
};
extern int ssm_lock_aains(ssm_Iaa aa, void *key, void *value);
extern void *ssm_lock_aarem(ssm_Iaa aa, void *key);
extern void *ssm_lock_aafind(ssm_Iaa aa, void *key);
extern int  ssm_lock_aadel(ssm_Iaa aa);


SSM_TYPE(lockl)
{
  ssm_Il_t l;
  ssm_Il   base;
  ssm_Ilock lock;
};
extern void *ssm_lock_ltop(ssm_Il l);
extern void *ssm_lock_lbot(ssm_Il l);
extern size_t ssm_lock_llen(ssm_Il l);
extern ssm_Ii ssm_lock_li(ssm_Il l);
extern int ssm_lock_lhas(ssm_Il l, void *data);
extern int ssm_lock_laddtop(ssm_Il l, void *data);
extern int ssm_lock_laddbot(ssm_Il l, void *data);
extern int ssm_lock_laddbef(ssm_Il l, void *data, void *anchor);
extern int ssm_lock_laddaft(ssm_Il l, void *data, void *anchor);
extern int ssm_lock_lrem(ssm_Il l, void *data);
extern void* ssm_lock_lremtop(ssm_Il l);
extern void* ssm_lock_lrembot(ssm_Il l);
extern int ssm_lock_ldel(ssm_Il l);

SSM_TYPE(lockcache)
{
  ssm_Icache_t cache;
  ssm_Icache base;
  ssm_Ilock lock;
};
extern int ssm_lock_cacheput(ssm_Icache cache, void *key, void *value, int size);
extern void *ssm_lock_cachefind(ssm_Icache cache, void *key);
extern void *ssm_lock_cacherem(ssm_Icache cache, void *key);
extern int ssm_lock_cachedel(ssm_Icache cache);

#if 0
SSM_TYPE(lockq)
{
  ssm_Iq_t q;
  ssm_Iq   base;
  ssm_Ilock lock;
};
#endif

#endif
