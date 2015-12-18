#ifndef SSM_REF_H
#define SSM_REF_H

#include <ssm.h>
#include <ssm/ll.h>
#include <ssm/spinlock.h>
#include <ssm/mutex.h>
#include <sched.h>

SSM_FLAGS(tx, TX, PLACEHOLDER);

SSM_TYPE(ssmhost)
{
  ssm_Ihost_t host;
  ssm_id     id;
};

SSM_OTYPE_DEFN(id)
{
  ssm_Il        mes;
  ssm_Il        pendtxs;
  ssm_Il        livetxs;
  ssm_Haddr     addr_local;
  ssm_Il        evs;  
  ssm_Fstart    flags;
  ssm_Itp       tp;
  ssm_Iaddr     addr;
  ssm_ssmhost   host;
  ssm_tpinfo    tpinfo;
  ssm_Ilock     waitlock;
  void          *xargs;
};

SSM_OTYPE_DEFN(me)
{
  ssm_id        owner;
  ssm_Il        bufs;
  ssm_bits      bits;
  ssm_bits      mask;
  ssm_cb        cb;
  ssm_Flink     flags;
};

SSM_TYPE(buf)
{
  ssm_me        owner;
  ssm_mmr_t     hdr;
  ssm_mmr_t     mmr;
  ssm_Fpost     flags;
};

SSM_TYPE(hdr)
{
  ssm_bits      bits;
  uint64_t      len;
  ssm_op        op;
} SSM_ATTR_PACKED;

static inline ssm_mr ssm_hdr_mr(ssm_bits bits, ssm_op op, ssm_mr inputmr)
{
  ssm_hdr hdr = SSM_NEW(hdr);
  bzero(hdr, SSM_SIZEOF(hdr));
  ssm_mrinfo_t mrinfo;
  ssm_mr_getinfo(&mrinfo, inputmr);
  hdr->bits = bits;
  hdr->len = ssm_mr_len(inputmr);
  hdr->op = op;
  return ssm_mr_create(NULL, hdr, SSM_SIZEOF(hdr));
}  
static inline int ssm_hdr_mrdel(ssm_mr hdrmr)
{
  ssm_mrinfo_t mrinfo;
  ssm_mr_getinfo(&mrinfo, hdrmr);
  ssm_mr_destroy(hdrmr);
  SSM_DELETE(mrinfo.base);
  return 0;
}


SSM_OTYPE_DEFN(tx)
{
  ssm_id        owner;
  ssm_cb        cb;
  ssm_me        me;
  ssm_op        op;
  ssm_buf       buf;
  ssm_md        md;
  ssm_bits      bits;
  ssm_Ftx       txflags;
  ssm_Haddr     addr;
  int           cancel;
};

#define SSM_REF_MATCH(a,b,m) ( ((a) & (~(m))) == ((b) & (~(m))) )

SSM_BENUM(mstate, MSG, INVALID, MISS, NOBUF, MATCH, DATA, END, FIND);

extern int ssm_delay(ssm_id id, long usecs);
extern int ssm_hold(ssm_id id);
extern int ssm_poll(ssm_id id);


extern int ssm_hostpost(ssm_Ihost host, ssm_Haddr addr, ssm_Il responses, ssm_Il head);
extern int ssm_hostrelease(ssm_Ihost host, ssm_Htx tx, ssm_md md, ssm_tpreply reply, ssm_tpact action);
extern int ssm_hostfinish(ssm_Ihost host, ssm_Htx tx, ssm_md md, ssm_tpreply reply, ssm_tpact action);
extern int ssm_hostdel(ssm_Ihost host);

static inline int ssm_doevent(ssm_id id)
{
  if(id->evs->top(id->evs))
  {
     ssm_ev ev = id->evs->remtop(id->evs);
     ssm_result result = ev->evdata;
     ssm_ev_consume(ev);
     ssm_hdr_mrdel(result->tx->buf->hdr.mr);
     SSM_DEL(result->tx->buf);
     SSM_DEL(result->tx);
     SSM_DEL(result);
     return 1;
  }
  return 0;
}

static inline int ssm_addevent(ssm_id id, ssm_tx tx, ssm_status status)
{
  ssm_dtrace();
  ssm_passert(id, tx);

  ssm_mdinfo_t info;
  ssm_md_getinfo(&info, tx->md);

  ssm_result result = SSM_NEW(result);
  result->id = id;
  result->me = tx->me;
  result->op = tx->op;
  result->md = tx->md;
  result->bits = tx->bits;
  result->mr = tx->buf->mmr.mr;
  result->bytes = info.len;
  result->addr = tx->addr;
  result->tx = tx;
  result->status = status;

  ssm_ev ev = ssm_cb_ev(tx->cb, result);
  ssm_rassert(ev);

  id->evs->addbot(id->evs, ev);
  return 0;
}

static inline ssm_mr ssm_iovs_mr(struct iovec *iovs, int len) 
{
  ssm_md md = NULL;
  int i;
  size_t max = 0;
  void *base = iovs->iov_base;
  for(i = 0; i < len; i++)
  {
    md = ssm_md_add(md, iovs->iov_base - base, iovs->iov_len);
    max = SSM_MAX(0, (iovs->iov_base - base) + iovs->iov_len);
  }
  ssm_mr mr = ssm_mr_create(md, base, max);
  return mr; 
}


#endif
