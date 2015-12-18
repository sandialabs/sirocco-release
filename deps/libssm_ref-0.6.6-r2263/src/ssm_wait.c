#include <ssm_ref.h>

int ssm_wait(ssm_id id, struct timeval *tv)
{
  if(!tv)
    return ssm_hold(id);
  unsigned long usecs = (tv->tv_sec * 1000000) + tv->tv_usec;
  if(usecs == 0)
    return ssm_poll(id);
  return ssm_delay(id, usecs);
}
