#include <ssm_dumb_ref.h>

void *ssm_dumb_aarem_fe(void *arg, void *data)
{
  ssm_dumbe e = data;
  ssm_ddtrace(p, arg, p, data, p, e->key, d, e->keylen);
  if(!memcmp(e->key, arg, e->keylen))
    return e;
  return NULL;
}

void * ssm_dumb_aarem(ssm_Iaa aa, void *key)
{
  ssm_ddtrace(p, aa, p, key);
  ssm_dumbaa daa = (ssm_dumbaa) aa;
  ssm_dumbe e = ssm_i_foreach(daa->l->i(daa->l), ssm_dumb_aarem_fe, key);
  if(e)
  {
    void *result = e->data;
    daa->l->rem(daa->l, e);
    SSM_DELETE(e);
    return result;
  }
  return NULL;
}
