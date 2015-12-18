#ifndef SSM_LL_REF_H
#define SSM_LL_REF_H

#include <ssm/ll.h>

SSM_TYPE_DECL(lln);

SSM_TYPE(ll)
{
  ssm_Il_t l;
  ssm_lln head;
  ssm_lln tail;
  ssm_Fll flags;
  size_t len;
}; 

SSM_TYPE_DEFN(lln)
{
  void *data;
  ssm_lln next;
  ssm_ll owner;
};
  

SSM_TYPE(lli)
{
  ssm_Ii_t i;
  ssm_lln nlln;
  ssm_ll owner;
  void *cur;
};


extern int ssm_ll_ldel(ssm_Il l);
extern int ssm_ll_laddtop(ssm_Il l, void *d);
extern int ssm_ll_laddbot(ssm_Il l, void *d);
extern int ssm_ll_laddbef(ssm_Il l, void *d, void *anchor);
extern int ssm_ll_laddaft(ssm_Il l, void *d, void *anchor);
extern int ssm_ll_lrem(ssm_Il l, void *ptr);
extern size_t ssm_ll_llen(ssm_Il l);
extern int ssm_ll_lhas(ssm_Il l, void *d);
extern void *ssm_ll_ltop(ssm_Il l);
extern void *ssm_ll_lbot(ssm_Il l);
extern void *ssm_ll_lremtop(ssm_Il l);
extern void *ssm_ll_lrembot(ssm_Il l);
extern ssm_Ii ssm_ll_i(ssm_Il l);

void * ssm_ll_icur(ssm_Ii lli);
void * ssm_ll_inext(ssm_Ii lli);
int ssm_ll_imore(ssm_Ii lli);
int ssm_ll_idel(ssm_Ii lli);

static inline int ssm_ll_ref_match(void *d, ssm_lln node)
{
  return (d == node->data);
}
#endif
