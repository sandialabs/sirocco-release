#ifndef __SSM_POSIXTCP_REF_H
#define __SSM_POSIXTCP_REF_H

#include <ssm/pbuf.h>
#include <ssm/lru.h>
#include <ssm/ll.h>
#include <ssm/cb.h>
#include <ssm/spinlock.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <ssmptcp.h>

#define SSMPTCP_MAXCONNS 4098
#define SSMPTCP_EAGERSIZE 1024
#define SSMPTCP_EVENTLEN 16

/*
 * As a server:
 * listen for incoming connections
 * accept and spin thread to listen on new fd
 * when get kill command, die
 *
 * As a client:
 * open outgoing connections and cache
 * when exit cache, send kill command and die
 */

SSMPTCP_TYPE(tp)
{
  ssm_Itp_t  tp;
  ssm_Ihost  host;
  uint64_t   active;
  uint16_t   port;
  ssm_Icache conns;
  ssm_Il     fail_conns;
  ssm_Il     extra_conns;
  ssm_Il     cleanup_conns;
  ssm_Il     txs;
  ssm_cb_t   dropcb;
  size_t     headmaxlen;
  int        connfd;
  int        ctlfd[2];
  int        ctl_len;
  struct epoll_event ctlfdev;
  struct epoll_event connfdev;
  int        epfd;
  struct epoll_event *epevs;
  int        epevlen;
};

SSMPTCP_TYPE(iaddr)
{
  ssm_Iaddr_t addr;
  ssmptcp_tp tp;
};

SSMPTCP_ENUM(op, OP, RPORT, PAIR, MD, DATA, REQ, DONE, ACK, STOP, DIE);
SSMPTCP_ENUM(call, CALL, UNKNOWN, PUSH, PULL, REMOTE);
SSMPTCP_ENUM(conn_st, CONN, READY, FAIL);
SSMPTCP_ENUM(tx_st, TX, INIT, SEND, WAIT, DONE, FAIL, DEAD);
SSMPTCP_ENUM(addr_origin, ADDR, LOCAL, REMOTE);

SSMPTCP_TYPE(addr)
{
  char host[20];
  uint16_t port;
  ssmptcp_addr_origin origin;
  
} SSMPP_ATTR_PACKED;


SSMPTCP_TYPE(conn)
{
  ssmptcp_tp tp;
  ssmptcp_addr_t addr;
  ssmptcp_conn_st st;
  ssmptcp_err err;
  struct epoll_event epev;
  uint32_t next_id;
  int fd;
  ssm_Il txs;
};

SSMPTCP_TYPE(tx)
{
  uint32_t      id;
  uint32_t      remote_id;
  ssmptcp_tx_st st;
  ssmptcp_err   err;
  ssmptcp_op    next_op;
  ssm_mr        local_mr;
  ssm_md        local_md;
  ssm_md        op_md;
  void *        op_md_buf;
  size_t        op_md_buf_len;
  ssmptcp_call  local_call;
  ssmptcp_call  remote_call;
  // SSM layer reply stuff
  ssm_tpreply   ssmreply;
  ssm_Htx       ssmtx;
};

SSMPTCP_TYPE(hdr)
{
  ssmptcp_op op;
  uint64_t a;
  uint64_t b;
  uint32_t id;
} SSMPP_ATTR_PACKED;

SSMPTCP_TYPE(packet)
{
  ssmptcp_hdr_t hdr;
  ssm_mr mr;
  ssm_md md;
};

extern int ssmptcp_addrcmp(ssm_Iaddr addr, ssm_Haddr a, ssm_Haddr b);
extern ssm_Haddr ssmptcp_addrcp(ssm_Iaddr addr, ssm_Haddr src);
extern size_t ssmptcp_addrcpv(ssm_Iaddr addr, ssm_Haddr src, ssm_Haddr *dest, int count);
extern ssm_Haddr ssmptcp_addrcreate(ssm_Iaddr addr, ssm_Haddrargs vargs);
extern int ssmptcp_addrdel(ssm_Iaddr addr);
extern int ssmptcp_addrdestroy(ssm_Iaddr addr, ssm_Haddr target);
extern ssm_Haddr ssmptcp_addrlocal(ssm_Iaddr addr);

extern ssm_Itp ssmptcp_new_tp(int port, ssmptcp_Fconn flags);
extern ssm_Iaddr ssmptcp_tpaddr(ssm_Itp tp);
extern int ssmptcp_tpdel(ssm_Itp tp);
extern int ssmptcp_tpmap(ssm_Itp tp, ssm_Il mmrs);
extern int ssmptcp_tppull(ssm_Itp vtp, ssm_Il txs, ssm_Haddr vaddr, ssm_Il heads, ssm_Il mmrs, ssm_Il mds);
extern int ssmptcp_tppush(ssm_Itp vtp, ssm_Il txs, ssm_Haddr vaddr, ssm_Il heads, ssm_Il mmrs, ssm_Il mds);
extern int ssmptcp_tpstart(ssm_Itp itp, ssm_Ihost host, ssm_tpinfo reply, size_t headmaxlen);
extern int ssmptcp_tpstop(ssm_Itp itp);
extern int ssmptcp_tpunmap(ssm_Itp tp, ssm_Il mmrs);
extern int ssmptcp_tpwait(ssm_Itp tp, long usec);
extern int ssmptcp_tpwake(ssm_Itp itp);

extern void ssmptcp_accept(ssmptcp_tp tp);
extern void ssmptcp_ctl(ssmptcp_tp tp);
extern void ssmptcp_dropcb(void *cbdat, void *evdat);
extern void ssmptcp_epadd(ssmptcp_tp tp, ssmptcp_conn conn);
extern void ssmptcp_epdel(ssmptcp_tp tp, ssmptcp_conn conn);
extern void ssmptcp_epmod(ssmptcp_tp tp, ssmptcp_conn conn);
extern void ssmptcp_event(ssmptcp_tp tp, struct epoll_event *epev);
extern void ssmptcp_sockaddr(struct sockaddr_in *saddr, ssmptcp_addr addr);

extern int ssmptcp_conn_epollin(ssmptcp_conn conn);
extern int ssmptcp_conn_epollout(ssmptcp_conn conn);
extern ssmptcp_conn ssmptcp_conn_find(ssmptcp_tp tp, ssmptcp_addr addr);
extern int ssmptcp_conn_finish(ssmptcp_conn conn);
extern ssmptcp_conn ssmptcp_conn_init(ssmptcp_tp tp, ssmptcp_addr addr);
extern void ssmptcp_conn_kill(ssmptcp_conn conn);
extern int ssmptcp_conn_op(ssmptcp_conn conn, ssmptcp_op op, uint32_t id, uint64_t a, uint64_t b);
extern int ssmptcp_conn_op_pair(ssmptcp_conn conn, uint64_t a, uint64_t b);
extern int ssmptcp_conn_op_rport(ssmptcp_conn conn, uint64_t a, uint64_t b);
extern int ssmptcp_conn_read(ssmptcp_conn conn, void *p, size_t len);
extern int ssmptcp_conn_readmr(ssmptcp_conn conn, ssm_mr mr, ssm_md md);
extern ssm_mr ssmptcp_conn_readmr_new(ssmptcp_conn conn, size_t len);
extern int ssmptcp_conn_readv(ssmptcp_conn conn, struct iovec *iovs, int len);
extern void ssmptcp_conn_recv(ssmptcp_conn conn, ssmptcp_op op, ssmptcp_tx tx, void *rsvp, uint64_t len);
extern int ssmptcp_conn_send(ssmptcp_conn conn, uint32_t remote_id, ssmptcp_op op, ssm_mr mr, ssm_md md, uint64_t a, uint64_t b);
extern int ssmptcp_conn_senders(ssmptcp_conn conn);
extern int ssmptcp_conn_write(ssmptcp_conn conn, void *p, size_t len);
extern int ssmptcp_conn_write_mr(ssmptcp_conn conn, ssm_mr mr, ssm_md md);
extern int ssmptcp_conn_writev(ssmptcp_conn conn, struct iovec *iovs, int len);

extern int ssmptcp_tx_abort(ssmptcp_tx tx);
extern int ssmptcp_tx_done(ssmptcp_tx tx);
extern int ssmptcp_tx_drop(ssmptcp_conn conn, ssmptcp_op op, uint64_t a, uint64_t b);
extern int ssmptcp_tx_finish(ssmptcp_tx tx, ssm_Ihost host);
extern int ssmptcp_tx_init(ssmptcp_tx tx, uint32_t id, uint32_t remote_id, ssm_mr local_mr, ssm_md local_md, ssmptcp_call local_call, ssm_tpreply reply, ssm_Htx ssmtx);
extern void ssmptcp_tx_kill(ssmptcp_tx tx);
extern int ssmptcp_tx_op(ssmptcp_tx tx, ssmptcp_conn conn, ssmptcp_op op, uint64_t a, uint64_t b);
extern int ssmptcp_tx_op_ack(ssmptcp_tx tx, ssmptcp_conn conn, uint64_t a, uint64_t b);
extern int ssmptcp_tx_op_data(ssmptcp_tx tx, ssmptcp_conn conn, uint64_t a, uint64_t b);
extern int ssmptcp_tx_op_done(ssmptcp_tx tx, ssmptcp_conn conn, uint64_t a, uint64_t b);
extern int ssmptcp_tx_op_md(ssmptcp_tx tx, ssmptcp_conn conn, uint64_t a, uint64_t b);
extern int ssmptcp_tx_op_req(ssmptcp_tx tx, ssmptcp_conn conn, uint64_t a, uint64_t b);
extern int ssmptcp_tx_op_stop(ssmptcp_tx tx, ssmptcp_conn conn, uint64_t a, uint64_t b);
extern int ssmptcp_tx_ready(ssmptcp_tx tx);
extern int ssmptcp_tx_send(ssmptcp_tx tx, ssmptcp_conn conn);
extern size_t ssmptcp_addr_serialize(ssm_Iaddr iaddr, char *out, size_t len, ssm_Haddr addr);
extern ssm_Haddr ssmptcp_addr_load(ssm_Iaddr addr, char *in, size_t len);

#endif
