#ifndef SSM_AA_H
#define SSM_AA_H

#include <ssm/ssm_pp.h>

SSM_INTERFACE_DECL(aa);

SSM_PROC(int, aains)(ssm_Iaa aa, void *key, void *value);
SSM_PROC(void *, aarem)(ssm_Iaa aa, void *key);
SSM_PROC(void *, aafind)(ssm_Iaa aa, void *key);
SSM_PROC(int, aadel)(ssm_Iaa aa);

SSM_INTERFACE_DEFN(aa)
{
  ssm_Paains ins;
  ssm_Paarem rem;
  ssm_Paafind find;
  ssm_Paadel del;
};


#endif
