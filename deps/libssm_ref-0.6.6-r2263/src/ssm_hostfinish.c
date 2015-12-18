#include <ssm_ref.h>

int ssm_hostfinish(ssm_Ihost host, ssm_Htx htx, ssm_md md, ssm_tpreply reply, ssm_tpact act)
{
  ssm_dtrace();
  ssm_ssmhost ssmhost = (ssm_ssmhost)host;

  ssm_tx tx = htx;
  tx->md = md;
  ssm_status status; 
  switch(reply)
  {
    case SSM_TPREPLY_NOMATCH: status = SSM_ST_NOMATCH; break;
    case SSM_TPREPLY_NOBUF: status = SSM_ST_NOBUF; break;
    case SSM_TPREPLY_CANCEL: status = SSM_ST_CANCEL; break;
    case SSM_TPREPLY_MATCH: status = SSM_ST_COMPLETE; break;
  }
  if(act == SSM_TPACT_FAILED) status = SSM_ST_NETFAIL;
  ssm_addevent(ssmhost->id, tx, status);
  return 0;
}

