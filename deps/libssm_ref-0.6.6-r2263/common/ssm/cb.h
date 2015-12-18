#ifndef SSM_CB_H
#define SSM_CB_H

SSM_PROC(void, cb)(void *cb_data, void *ev_data);
SSM_HANDLE(cbdata, evdata);

SSM_TYPE(cb)
{
  ssm_Pcb pcb;
  ssm_Hcbdata cbdata;
};

SSM_TYPE(ev)
{
  ssm_cb cb;
  ssm_Hevdata evdata;
};

static inline void ssm_cb_invoke(ssm_cb cb, void *evdata)
{
  if(cb->pcb)
    cb->pcb(cb->cbdata, evdata);
}

static inline ssm_ev ssm_cb_ev(ssm_cb cb, void *evdata)
{
  ssm_ev result = SSM_NEW(ev);
  result->cb = cb;
  result->evdata = evdata;
  return result;
}

static inline void ssm_ev_consume(ssm_ev ev)
{
  ssm_cb_invoke(ev->cb, ev->evdata);
  SSM_DEL(ev);
}

#endif
