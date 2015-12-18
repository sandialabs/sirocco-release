#include <ssm_mutex_ref.h>

ssm_Ilock ssm_mutex_lock()
{
  ssm_mutexlock ml = SSM_NEW(mutexlock);
  ml->lock.arm = ssm_mutex_lockarm;
  ml->lock.share = ssm_mutex_lockshare;
  ml->lock.rel = ssm_mutex_lockrel;
  ml->lock.del = ssm_mutex_lockdel;
  pthread_mutex_init(&(ml->ptmutex), 0);
  return (ssm_Ilock)ml;
}

