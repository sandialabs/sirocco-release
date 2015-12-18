#include <ssm_ref.h>

int ssm_post(ssm_id id, ssm_me me, ssm_mr mr, ssm_Fpost flags)
{
  ssm_passert(id, me, me->owner == id);

  ssm_buf buf = SSM_NEW(buf);
  buf->owner = me;
  buf->mmr.mr = mr;
  buf->mmr.flags = SSM_NOF;
  buf->hdr.mr = ssm_hdr_mr(me->bits, SSM_OP_UNLINK, mr);
  buf->hdr.flags = SSM_MAP_HEAD;
  buf->flags = flags;

  ssm_dprint(p, buf, p, buf->mmr.mr, p, buf->hdr.mr);
  ssm_Il l = ssm_ll_l(SSM_NOF);
  l->addtop(l, &(buf->mmr));
  l->addtop(l, &(buf->hdr));
  id->tp->map(id->tp, l);
  l->del(l);

  me->bufs->addbot(me->bufs, buf);
  return 0;
}
