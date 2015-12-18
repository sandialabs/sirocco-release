#include <ssm_lock_ref.h>

ssm_Il ssm_lock_l(ssm_Ilock lock, ssm_Il l)
{
  ssm_lockl lockl = SSM_NEW(lockl);

  lockl->l.top = ssm_lock_ltop;
  lockl->l.bot = ssm_lock_lbot;
  lockl->l.i = ssm_lock_li;
  lockl->l.has = ssm_lock_lhas;
  lockl->l.addtop = ssm_lock_laddtop;
  lockl->l.addbot = ssm_lock_laddbot;
  lockl->l.addbef = ssm_lock_laddbef;
  lockl->l.addaft = ssm_lock_laddaft;
  lockl->l.rem = ssm_lock_lrem;
  lockl->l.remtop = ssm_lock_lremtop;
  lockl->l.rembot = ssm_lock_lrembot;
  lockl->l.del = ssm_lock_ldel;
  lockl->l.len = ssm_lock_llen;
  lockl->base = l;
  lockl->lock = lock;

  return (ssm_Il)lockl;
}


