#ifndef __SSM_LOCK
#define __SSM_LOCK

#include <stdint.h>
#include <ssm/ssm_pp.h>
#include <ssm/if/i.h>
#include <ssm/if/aa.h>
#include <ssm/if/l.h>
#include <ssm/if/cache.h>

SSM_INTERFACE_DECL(lock);

SSM_PROC(int, lockarm)(ssm_Ilock lock);
SSM_PROC(int, lockshare)(ssm_Ilock lock);
SSM_PROC(int, lockrel)(ssm_Ilock lock);
SSM_PROC(int, lockdel)(ssm_Ilock lock);

SSM_INTERFACE_DEFN(lock)
{
  ssm_Plockarm     arm;
  ssm_Plockshare   share;
  ssm_Plockrel     rel;
  ssm_Plockdel     del;
};

#if 0
SSM_FLAGS(lock, LOCK, (SPIN)(RW));
SSM_OTYPE(lock);
extern int ssm_lock_init(ssm_lock lock, ssm_Flock flags);
extern ssm_lock ssm_lock_new(ssm_Flock flags);
extern int ssm_lock_kill(ssm_lock lock);
extern int ssm_lock_del(ssm_lock lock);
extern int ssm_lock_arm(ssm_lock lock);
extern int ssm_lock_arr(ssm_lock lock);
extern int ssm_lock_rel(ssm_lock lock);
#endif

// extended lock API

extern ssm_Ii  ssm_lock_i(ssm_Ilock lock, ssm_Ii i);
extern ssm_Iaa ssm_lock_aa(ssm_Ilock lock, ssm_Iaa aa);
extern ssm_Il  ssm_lock_l(ssm_Ilock lock, ssm_Il l);
extern ssm_Icache ssm_lock_cache(ssm_Ilock lock, ssm_Icache cache);
//extern ssm_Iq  ssm_lock_q(ssm_Ilock lock, ssm_Iq q);

#endif
