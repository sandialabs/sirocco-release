#ifndef __SSM_SPINLOCK_REF_H
#define __SSM_SPINLOCK_REF_H

#include <ssm/spinlock.h>
#include <pthread.h>

SSM_TYPE(spinlocklock)
{
  ssm_Ilock_t lock;
  pthread_spinlock_t psl;
};

extern int ssm_spinlock_lockarm(ssm_Ilock lock);
extern int ssm_spinlock_lockshare(ssm_Ilock lock);
extern int ssm_spinlock_lockrel(ssm_Ilock lock);
extern int ssm_spinlock_lockdel(ssm_Ilock lock);

#endif
