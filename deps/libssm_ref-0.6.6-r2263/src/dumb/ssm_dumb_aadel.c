#include <ssm_dumb_ref.h>

int ssm_dumb_aadel(ssm_Iaa aa)
{
  ssm_dumbaa daa = (ssm_dumbaa) aa;
  ssm_dumbe e;
  while(daa->l->top(daa->l))
  {
    e = daa->l->remtop(daa->l);
    SSM_DELETE(e);
  }
  daa->l->del(daa->l);
  SSM_DEL(aa);
  return 0;
}
