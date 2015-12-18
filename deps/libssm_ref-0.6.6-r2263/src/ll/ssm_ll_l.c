#include <ssm_ll_ref.h>

ssm_Il ssm_ll_l(ssm_Fll flags)
{
  ssm_ll ll = SSM_NEW(ll);
  ll->l.top = ssm_ll_ltop;
  ll->l.bot = ssm_ll_lbot;
  ll->l.i = ssm_ll_i;
  ll->l.has = ssm_ll_lhas;
  ll->l.len = ssm_ll_llen;
  ll->l.addtop = ssm_ll_laddtop;
  ll->l.addbot = ssm_ll_laddbot;
  ll->l.addbef = ssm_ll_laddbef;
  ll->l.addaft = ssm_ll_laddaft;
  ll->l.rem    = ssm_ll_lrem;
  ll->l.remtop = ssm_ll_lremtop;
  ll->l.rembot = ssm_ll_lrembot;
  ll->l.del    = ssm_ll_ldel;
  ll->flags = flags;
  ll->head = NULL;
  ll->tail = NULL;
  ll->len = 0;
  return (ssm_Il)ll;
}
