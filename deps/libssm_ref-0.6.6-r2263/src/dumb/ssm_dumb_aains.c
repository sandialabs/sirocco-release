#include <ssm_dumb_ref.h>

int ssm_dumb_aains(ssm_Iaa aa, void *key, void *value)
{
  ssm_dumbaa daa = (ssm_dumbaa) aa;
  ssm_dumbe e = SSM_NEW(dumbe);
  e->data = value;
  e->key = key;
  e->keylen = daa->keylen;
  daa->l->addbot(daa->l, e);
  return 0;
}
