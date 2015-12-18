#ifndef SSM_EV_REF_H
#define SSM_EV_REF_H

#include <ssm/ssm_ev.h>
#include <ssm/ssm_cb.h>

struct _ssm_ev
{
  ssm_cb cb;
  void *data;
  ssm_ev_flags flags;
};

#endif
