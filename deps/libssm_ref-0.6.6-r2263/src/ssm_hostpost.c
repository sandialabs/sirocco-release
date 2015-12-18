#include <ssm_ref.h>

SSM_TYPE(ssmpost_fe_matchargs)
{
  ssm_Il respl;
  ssm_id id;
  ssm_Haddr addr;
};

void * ssm_ssmpost_fe_match(void *ibits, void *ime)
{
  ssm_dtrace();
  ssm_bits bits = *((ssm_bits *)ibits);
  ssm_me me = ime;
  //if(SSM_REF_MATCH(bits, me->bits, me->mask) && me->bufs->len(me->bufs))
  if(SSM_REF_MATCH(bits, me->bits, me->mask)) // MSF re: #505
    return me;
  ssm_dprint(lit, "failed match: ", x, bits, x, me->bits, x, me->mask);
  return NULL;
}

void* ssm_ssmpost_fe_walk(void *matchargs, void *ihdr)
{
  ssm_dtrace();
  ssm_ssmpost_fe_matchargs args = matchargs;
  ssm_id id = args->id;
  ssm_mmr hdrmmr = ihdr;
  ssm_mr hdrmr = hdrmmr->mr;
  ssm_mrinfo_t mrinfo;
  ssm_mr_getinfo(&mrinfo, hdrmr);
  ssm_hdr hdr = mrinfo.base;
  ssm_tpr tpr = SSM_NEW(tpr);
  ssm_me me = ssm_i_foreach(args->id->mes->i(args->id->mes), ssm_ssmpost_fe_match, &(hdr->bits));
  if(me)
  {
    ssm_buf buf = me->bufs->top(me->bufs);
    int willfit = 0;
    if(buf)
    {
      ssm_dprint(lit, "match: ", p, buf, p, buf->mmr.mr, p, hdr, d, hdr->len, d, buflen);
      size_t buflen = ssm_mr_len(buf->mmr.mr);
      switch(hdr->op)
      {
        case SSM_OP_PUT:  if(hdr->len <= buflen) willfit = 1; break;
        case SSM_OP_GET:  if(hdr->len >= buflen) willfit = 1; break;
        default: break;
      }
    }
    if(willfit)
    {
      ssm_dprint(lit, "will fit");
      ssm_tx tx = SSM_NEW(tx);
      tx->owner = me->owner;
      tx->me = me;
      tx->cb = me->cb;
      tx->op = hdr->op;
      
      tx->buf = me->bufs->top(me->bufs);
      if(!(tx->buf->flags & SSM_POST_STATIC))
      {
        me->bufs->remtop(me->bufs);
        if((me->bufs->len(me->bufs) == 0) && (me->flags & SSM_LINK_AUTO_UNLINK))
        {
          ssm_unlink(me->owner, me);
          ssm_result r = SSM_NEW(result);
          r->id = me->owner;
          r->me = me;
          r->tx = NULL;
          r->bits = me->bits;
          r->status = SSM_ST_COMPLETE;
          r->op = SSM_OP_UNLINK;
          r->addr = NULL;
          r->mr = NULL;
          r->md = NULL;
          r->bytes = 0;
          me->owner->evs->addbot(me->owner->evs, ssm_cb_ev(me->cb, r));
        }
      }
      tx->md = NULL;
      tx->bits = hdr->bits;
      tx->txflags = SSM_NOF;
      tx->addr = args->addr;
      tx->cancel = 0;
      id->livetxs->addbot(id->livetxs, tx);
      tpr->tx = tx;
      tpr->mmr = &(tx->buf->mmr);
      tpr->reply = SSM_TPREPLY_MATCH;
    }
    else
    {
      ssm_dprint(lit, "won't fit");
      tpr->tx = NULL;
      tpr->mmr = NULL;
      tpr->reply = SSM_TPREPLY_NOBUF;
    }
  }
  else
  {
    ssm_dprint(lit, "no match");
    tpr->tx = NULL;
    tpr->mmr = NULL;
    tpr->reply = SSM_TPREPLY_NOMATCH;
  }
  ssm_ddprint(p, tpr);
  args->respl->addbot(args->respl, tpr);
  return NULL;
}

int ssm_hostpost(ssm_Ihost host, ssm_Haddr addr, ssm_Il responses, ssm_Il head)
{
  ssm_dtrace(p, head, d, head->len(head));
  ssm_ssmhost ssmhost = (ssm_ssmhost) host;
  ssm_ssmpost_fe_matchargs_t matchargs;
  matchargs.respl = responses;
  matchargs.id = ssmhost->id;
  matchargs.addr = addr;
  ssm_i_foreach(head->i(head), ssm_ssmpost_fe_walk, &matchargs);
  return 0;
}
