#include <ssm_ref.h>

int ssm_hold(ssm_id id)
{
  int done = ssm_poll(id);
  while(ssm_update(id));
  while(!done)
  {
    if(id->tp)
      id->tp->wait(id->tp, -1);
    while(ssm_update(id));
    done = ssm_poll(id);
  }
  return 0;
}
