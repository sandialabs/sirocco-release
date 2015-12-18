#ifndef __SSM_POSIXTCP_H
#define __SSM_POSIXTCP_H

#include <ssm.h>

#define SSMPTCP_TYPE(name) \
  SSMPP_TYPE(ssmptcp, name)

#define SSMPTCP_ENUM(name, cname, ...) \
  SSMPP_ENUM(SSMPP_ENUM_I, ssmptcp, SSMPTCP, name, cname, __VA_ARGS__)

#define SSMPTCP_FLAGS(name, cname, ...) \
  SSMPP_FLAGS(ssmptcp, SSMPTCP, uint64_t, SSMPP_CAT(F,name), cname, __VA_ARGS__)

#define SSMPTCP_SIZEOF(type) \
  sizeof(SSMPP_TYPE_VNAME(ssmptcp, type))

#define SSMPTCP_NEW(type) \
  SSM_TMALLOC(SSMPP_TYPE_VNAME(ssmptcp, type))

SSMPTCP_TYPE(addrargs)
{
  int port;
  char *host;
};

SSMPTCP_FLAGS(conn, CONN, NOLINGER);

SSMPTCP_ENUM(err, ERR, OK, INVALID_ADDR, READ_FAIL, WRITE_FAIL, INTERNAL, SOMETHING_WEIRD);

extern ssm_Itp ssmptcp_new_tp(int port, ssmptcp_Fconn flags);
extern char *ssmptcp_addr_dump(ssm_Haddr addr);

#endif
