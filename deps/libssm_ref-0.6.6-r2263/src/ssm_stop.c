#include <ssm_ref.h>

int ssm_stop(ssm_id id)
{
  id->tp->stop(id->tp);
  id->addr->del(id->addr);
  SSM_DELETE(id->tpinfo);
  id->pendtxs->del(id->pendtxs);
  id->livetxs->del(id->livetxs);
  SSM_DELETE(id->host);
  id->mes->del(id->mes);
  id->evs->del(id->evs);
  id->waitlock->del(id->waitlock);
  SSM_DELETE(id);
  return 0;


}
