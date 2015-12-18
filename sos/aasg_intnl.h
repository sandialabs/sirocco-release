#include <pthread.h>
#include <sys/queue.h>
#include "sos_prot.h"

/*
 * We make requests against this match entry identifier.
 */
#ifndef AASG_UXID
# define AASG_UXID		((xm_match_t )0)
#endif

/*
 * Size of the raw buffer used for aerializing/desieralizing wire data.
 */
#ifndef AASG_RAWBUFSIZ
# define AASG_RAWBUFSIZ		1000U
#endif

struct WRITE1ret {
	asg_update_id_t *new_update_id;
	asg_size_t *transferred;
};

struct READ1ret {
	asg_size_t bufsiz;
	asg_size_t rbufsiz;
	asg_size_t *transferred;
	asg_size_t *record_buf_transferred;
	asg_update_id_t *update_id_info;
	asg_record_info_t *record_buf;
};

struct BATCH1ret {
	sosstat1 *ret;
};

typedef struct WRITE1ret WRITE1ret;
typedef struct READ1ret READ1ret;
typedef struct BATCH1ret BATCH1ret;

union aasg_cooked {
	union {
		SOSargs SOS;
		BATCH1args b;
	} request;
	union {
		SOSres SOS;
		BATCH1res b;
	} result;
};

struct context {
	TAILQ_ENTRY(context) _link;
	struct asg_batch_id_t *bid;
	union aasg_cooked u;
	union {
		WRITE1ret WRITE1;
		READ1ret READ1;
		BATCH1ret BATCH1;
	} retv;
	size_t len;
	struct aasg_rdma_buf *rdma;
	int error;
	void (* setretv)(struct context *);
};

#define AASG_GRPID_REPLY	0U
#define AASG_NGRP               16
#define AASG_DEPTH              4U
#define AASG_CALLS_MAX          64U

struct aasg_transport {
	struct xm_transport *xmx;
	struct xm_match_entry *grp[AASG_NGRP][AASG_CALLS_MAX][AASG_DEPTH];
	int map[AASG_NGRP];
	pthread_mutex_t m, mp;
	pthread_cond_t sig;
	pthread_t waiter;
	int shutdown;
};

#define AASG_OPS 7U

struct asg_instance {
	void (* handler[AASG_OPS])(void *, asg_result_t);
};

typedef struct asg_instance asg_instance;

struct aasg_peer {
	struct aasg_transport *aasgx;
	ssm_Haddr addr;
};

struct aasg_request_header {
	uint32_t proc;
	uint64_t reply;
};

struct asg_batch_id_t {
	TAILQ_HEAD(, context) op_queue;
	struct context *ctx;
	int id;
	uint64_t bid;
	struct aasg_request_header header;
	char raw_header[12];
	void *cb_data;
	void (*callback)(void *, asg_result_t);
	void (*setret)(struct context *);
	void (*completion_handler)(struct asg_batch_id_t *);
	char raw[AASG_RAWBUFSIZ];
	struct xm_message msg[2];
	struct iovec iov[3];
	int error;
	size_t mbitc;
	struct asg_instance *inst;
	struct aasg_peer *aasgp;
	void *cmdbuf;
	size_t len;
};

struct aasg_rdma_buf {
	unsigned pinned;
	unsigned active;
	struct aasg_peer *aasgp;
	struct iovec iov[1];
	struct xm_message message;
	void (*setretv)(struct context *ctx);
	struct context *ctx;
	size_t pos;
};

/*
 * Wasteful... Should check if we have the builtin and use that instead when we
 * can.
 */
#define ALIGNOF(__ty) \
	sizeof(__ty)

#define MBITC_RBITS		1U
#define MBITC_MAPBITS		8U

#define MBITC_NRBITS \
	(sizeof(xm_match_t) * CHAR_BIT - (MBITC_RBITS + MBITC_MAPBITS))

#define MBITC_NRSHIFT		0U
#define MBITC_MAPSHIFT		(MBITC_NRSHIFT + MBITC_NRBITS)
#define MBITC_RSHIFT		(MBITC_MAPSHIFT + MBITC_MAPBITS)

#define MBITC_NRMASK		(((xm_match_t )1 << MBITC_NRBITS) - 1)
#define MBITC_MAPMASK		(((xm_match_t )1 << MBITC_MAPBITS) - 1)
#define MBITC_RMASK		(((xm_match_t )1 << MBITC_RBITS) - 1)

#define MBITC(__mapid, __nr)		  \
	((((1U) & MBITC_RMASK) << MBITC_RSHIFT) | \
	 (((__mapid) & MBITC_MAPMASK) << MBITC_MAPSHIFT) | \
	 (((__nr) & MBITC_NRMASK) << MBITC_NRSHIFT))

#define MBITC_MAP(__nr)		(((__nr) >> MBITC_MAPSHIFT) & MBITC_MAPMASK)
#define MBITC_NR(__nr)		(((__nr) >> MBITC_NRSHIFT) & MBITC_NRMASK)

extern void _aasg_set_nsid1(nsid1 *ident,
			    asg_container_id_t container,
			    asg_object_id_t object,
			    asg_fork_id_t fork);
extern void _aasg_complete(struct asg_batch_id_t *bid);
extern int _aasg_setflags(asg_flags_t flags, cond1 *c);
extern int _aasg_bidalloc(struct aasg_peer *aasgp,
			  void (*completion_handler)(struct asg_batch_id_t *),
			  struct asg_batch_id_t **bid);
extern void _aasg_bidfree(struct asg_batch_id_t *bid);
extern int _aasg_rdma_post(struct aasg_peer *aasgp,
			   unsigned flags,
			   xm_match_t match,
			   const struct iovec *iov,
			   size_t iovcnt,
			   void (*callback)(struct xm_transport *,
					    struct xm_message *),
			   void *data,
			   struct xm_message *xmmsg,
			   size_t pos);
extern int _aasg_rdma_cancel(struct aasg_peer *aasgp, struct xm_message *xmmsg,
			     size_t pos);
extern int _aasg_rdma_post_buf(struct aasg_peer *aasgp,
			       unsigned flags,
			       xm_match_t match,
			       struct aasg_rdma_buf *aasgrb);
extern int _aasg_rdma_cancel_buf(struct aasg_rdma_buf *aasgrb);
extern ssize_t _aasg_serdes(xdrproc_t xdrproc,
			    void *cooked,
			    void *raw, size_t size,
			    enum xdr_op xop);
extern ssize_t _aasg_pserdes(struct asg_batch_id_t *bid,
			     xdrproc_t xdrproc,
			     void *cooked,
			     enum xdr_op xop);
extern void _aasg_ignore(struct xm_transport *xmx, struct xm_message *xmmsgs);
extern int _aasg_request(struct asg_batch_id_t *aasgctx,
			 uint32_t proc,
			 void *args, size_t sizargs,
			 void *res, size_t sizres);
extern int _asg_batch_trigger(struct asg_batch_id_t *bid,
			      asg_time_t *hoq,
			      struct context *ctx);
extern int bstat(struct context *ctx);

