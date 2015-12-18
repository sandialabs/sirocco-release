#ifndef __SSM_L_H
#define __SSM_L_H

#include <ssm/ssm_pp.h>
#include <ssm/if/i.h>

SSM_INTERFACE_DECL(l);

SSM_PROC(void *, ltop)(ssm_Il l);
SSM_PROC(void *, lbot)(ssm_Il l);
SSM_PROC(ssm_Ii, li)(ssm_Il l);
SSM_PROC(size_t, llen)(ssm_Il l);
SSM_PROC(int, lhas)(ssm_Il l, void * data);
SSM_PROC(int, laddtop)(ssm_Il l, void *data);
SSM_PROC(int, laddbot)(ssm_Il l, void *data);
SSM_PROC(int, laddbef)(ssm_Il l, void *data, void *anchor);
SSM_PROC(int, laddaft)(ssm_Il l, void *data, void *anchor);
SSM_PROC(int, lrem)(ssm_Il l, void *data);
SSM_PROC(void *, lremtop)(ssm_Il l);
SSM_PROC(void *, lrembot)(ssm_Il l);
SSM_PROC(int, ldel)(ssm_Il l);

SSM_INTERFACE_DEFN(l)
{
  ssm_Pltop top;
  ssm_Plbot bot;
  ssm_Pli    i;
  ssm_Pllen  len;
  ssm_Plhas  has;
  ssm_Pladdtop addtop;
  ssm_Pladdbot addbot;
  ssm_Pladdbef addbef;
  ssm_Pladdaft addaft;
  ssm_Plrem    rem;
  ssm_Plremtop remtop;
  ssm_Plrembot rembot;
  ssm_Pldel    del;
};


#define SSM_IL_TYPESAFE_PROCNAME(type) SSM_CAT()

#define SSM_IL_TYPESAFE_DECLS(listtype) 
  

#endif
