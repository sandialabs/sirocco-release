#include <ssm_ref.h>

static long ssm_timediff(struct timeval *start, struct timeval *now)
{
  long diff;
  diff = (now->tv_sec - start->tv_sec)  * 1000000;
  // if now.usec - start.usec is negative, this still does what we want
  diff += (now->tv_usec - start->tv_usec);
  return diff;
}

int ssm_delay(ssm_id id, long usecs)
{
  ssm_dtrace(p, id, d, usecs);
  int poll_result;
  struct timeval start, now;
  gettimeofday(&start, NULL);
  id->waitlock->arm(id->waitlock);
  gettimeofday(&now, NULL);
  long diff = ssm_timediff(&start, &now);
  while(diff < usecs)
  { 
    poll_result = ssm_poll(id);
    if(poll_result)
    {
      ssm_dreturn(lit, "found event", d, poll_result); 
      id->waitlock->rel(id->waitlock);
      return poll_result;
    }
    gettimeofday(&now, NULL);
    diff = ssm_timediff(&start, &now);
    if(id->tp && (usecs > diff) && (usecs > 0))
      id->tp->wait(id->tp, usecs - diff);
  }
  id->waitlock->rel(id->waitlock);
  ssm_dreturn(lit, "timed out", d, poll_result); 
  return 0;
}

