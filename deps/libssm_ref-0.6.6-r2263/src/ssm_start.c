#include <ssm_ref.h>

ssm_id ssm_start(ssm_Itp tp, ssm_Haddrargs addrargs, ssm_Fstart flags)
{
  ssm_passert(tp);
  ssm_id result = SSM_NEW(id);
  result->mes = ssm_lock_l(ssm_spinlock_lock(), ssm_ll_l(SSM_NOF));
  result->host = SSM_NEW(ssmhost);
  result->host->host.post = ssm_hostpost;
  result->host->host.release = ssm_hostrelease;
  result->host->host.finish = ssm_hostfinish;
  result->host->id = result;
  result->tp = tp;
  result->addr = result->tp->addr(result->tp);
  result->flags = flags;
  result->evs = ssm_lock_l(ssm_spinlock_lock(), ssm_ll_l(SSM_NOF));
  result->tpinfo = SSM_NEW(tpinfo);
  result->pendtxs = ssm_lock_l(ssm_spinlock_lock(), ssm_ll_l(SSM_NOF));
  result->livetxs = ssm_lock_l(ssm_spinlock_lock(), ssm_ll_l(SSM_NOF));
  result->waitlock = ssm_mutex_lock();

  result->tp->start(result->tp, (ssm_Ihost)result->host, result->tpinfo, SSM_SIZEOF(hdr));
  return result;
}
