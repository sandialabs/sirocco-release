#include <ssm_mutex_ref.h>

int ssm_mutex_lockdel(ssm_Ilock lock)
{
  ssm_mutexlock ml = (ssm_mutexlock)lock;
  pthread_mutex_destroy(&(ml->ptmutex));
  SSM_DELETE(ml);
  return 0;
}

