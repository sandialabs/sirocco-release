#include <ssm_dumb_ref.h>

void *ssm_dumb_aafind_fe(void *arg, void *data)
{
  ssm_dumbe e = data;
  if(!memcmp(e->key, arg, e->keylen))
    return e->data;
  return NULL;
}

void * ssm_dumb_aafind(ssm_Iaa aa, void *key)
{
  ssm_dumbaa daa = (ssm_dumbaa) aa;
  return ssm_i_foreach(daa->l->i(daa->l), ssm_dumb_aafind_fe, key);
}

