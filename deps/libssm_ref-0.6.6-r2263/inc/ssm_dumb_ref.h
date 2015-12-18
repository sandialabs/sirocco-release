#ifndef __SSM_DUMB_REF_H
#define __SSM_DUMB_REF_H

#include <ssm.h>
#include <ssm/dumb.h>
#include <ssm/ll.h>

SSM_TYPE(dumbaa)
{
  ssm_Iaa_t aa;
  ssm_Il l;
  size_t keylen;
};

SSM_TYPE(dumbe)
{
  void *data;
  void *key;
  size_t keylen;
};

extern int ssm_dumb_aains(ssm_Iaa aa, void *key, void *value);
extern void * ssm_dumb_aarem(ssm_Iaa aa, void *key);
extern void * ssm_dumb_aafind(ssm_Iaa aa, void *key);
extern int ssm_dumb_aadel(ssm_Iaa aa);

#endif
