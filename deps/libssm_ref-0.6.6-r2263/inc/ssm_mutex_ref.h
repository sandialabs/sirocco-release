#ifndef __SSM_MUTEX_REF_H
#define __SSM_MUTEX_REF_H

#include <ssm/mutex.h>
#include <pthread.h>

SSM_TYPE(mutexlock)
{
  ssm_Ilock_t lock;
  pthread_mutex_t ptmutex;
};

extern int ssm_mutex_lockarm(ssm_Ilock lock);
extern int ssm_mutex_lockshare(ssm_Ilock lock);
extern int ssm_mutex_lockrel(ssm_Ilock lock);
extern int ssm_mutex_lockdel(ssm_Ilock lock);



#endif
