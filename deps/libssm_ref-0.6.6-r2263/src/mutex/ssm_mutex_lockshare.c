#include <ssm_mutex_ref.h>

int ssm_mutex_lockshare(ssm_Ilock lock)
{
  ssm_mutexlock ml = (ssm_mutexlock) lock;
  pthread_mutex_lock(&(ml->ptmutex));
  return 0;
}
