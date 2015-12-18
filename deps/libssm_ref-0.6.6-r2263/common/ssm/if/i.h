#ifndef __SSM_I_H
#define __SSM_I_H

// ssm iterator

#include <ssm/ssm_pp.h>

SSM_INTERFACE_DECL(i);

SSM_PROC(void *, icur)(ssm_Ii i);
SSM_PROC(void *, inext)(ssm_Ii i);
SSM_PROC(int, imore)(ssm_Ii i);
SSM_PROC(int, idel)(ssm_Ii i);

SSM_HANDLE(iprv, idata);

SSM_INTERFACE_DEFN(i)
{
  ssm_Picur     cur;
  ssm_Pinext    next;
  ssm_Pimore    more;
  ssm_Pidel     del;
};

SSM_PROC(void *, iforeach)(void *arg, void *data);
static inline void * ssm_i_foreach(ssm_Ii i, ssm_Piforeach pife, void *arg)
{
  ssm_dtrace(p, i, p, pife, p, arg);
  void *result = NULL;
  while(i->more(i))
  {
    result = pife(arg, i->next(i));
    if(result) break;
  }
  i->del(i);
  ssm_ddprint(p, result);
  return result;
}
#endif
