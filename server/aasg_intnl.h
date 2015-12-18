#ifndef __ASG_INTNL_H__
#define __ASG_INTNL_H__
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

/*
 * Maximum number of simultaneous outstanding wire transactions to
 * a given peer.
 */
#ifndef AASG_CALLS_MAX
# define AASG_CALLS_MAX		15U
#endif

union aasg_cooked {
	union {
	       	SOSargs SOS;
		BATCH1args BATCH;
	} request;
	union {
		SOSres SOS;
		BATCH1res BATCH;
	} result;
};

#if 0
#define AASG_GRPID_REPLY	0U
#define AASG_GRPID_BUF		1U
#define AASG_GRPID_BUFA		2U

#define AASG_NGRP		3

struct aasg_transport {
	struct xm_transport *xmx;
	struct xm_match_entry *grp[AASG_NGRP][AASG_CALLS_MAX];
};

struct aasg_peer {
	struct aasg_transport *aasgx;
	const struct xm_address *addr;
	int	map[(AASG_CALLS_MAX + (CHAR_BIT - 1)) & ~(CHAR_BIT - 1)];
};

struct aasg_request_header {
	uint32_t proc;
	uint64_t reply;
};

struct aasg_context {
	struct aasg_peer *aasgp;
	int	id;
	struct aasg_request_header header;
	char	raw_header[12];
	void	(*completion_handler)(void *);
	void	*data;
	char	raw[AASG_RAWBUFSIZ];
	struct iovec iov[3];
	struct xm_message msg[2];
	int	error;
};

/*
 * Wasteful... Should check if we have the builtin and use that instead when we
 * can.
 */
#define ALIGNOF(__ty) \
	sizeof(__ty)

#define MBITC_RBITS		1U
#ifndef MBITC_MAPBITS
# define MBITC_MAPBITS		8U
#endif
#define MBITC_NRBITS \
	(sizeof(xm_match_t) * CHAR_BIT - (MBITC_RBITS + MBITC_MAPBITS))

#define MBITC_NRSHIFT		0U
#define MBITC_MAPSHIFT		(MBITC_NRSHIFT + MBITC_NRBITS)
#define MBITC_RSHIFT		(MBITC_MAPSHIFT + MBITC_MAPBITS)

#define MBITC_NRMASK		(((xm_match_t )1 << MBITC_NRBITS) - 1)
#define MBITC_MAPMASK		(((xm_match_t )1 << MBITC_MAPBITS) - 1)
#define MBITC_RMASK		(((xm_match_t )1 << MBITC_RBITS) - 1)

#define MBITC(__mapid, __nr) \
	((((1U) & MBITC_RMASK) << MBITC_RSHIFT) | \
	 (((__mapid) & MBITC_MAPMASK) << MBITC_MAPSHIFT) | \
	 (((__nr) & MBITC_NRMASK) << MBITC_NRSHIFT))

#define MBITC_MAP(__nr)		(((__nr) >> MBITC_MAPSHIFT) & MBITC_MAPMASK)
#define MBITC_NR(__nr)		(((__nr) >> MBITC_NRSHIFT) & MBITC_NRMASK)

extern void _aasg_set_nsid1(nsid1 *ident,
			    asg_container_id_t container,
			    asg_object_id_t object,
			    asg_fork_id_t fork);
extern int _aasg_ctxalloc(struct aasg_peer *aasgp,
			  size_t xalign,
			  size_t xsize,
			  void (*completion_handler)(void *),
			  struct aasg_context **aasgctxp);
extern void _aasg_ctxfree(struct aasg_context *aasgctx);
extern int _aasg_rdma_post(struct aasg_peer *aasgp,
			   unsigned flags,
			   xm_match_t match,
			   const struct iovec *iov,
			   size_t iovcnt,
			   void (*callback)(struct xm_transport *,
					    struct xm_message *),
			   void *data,
			   struct xm_message *xmmsg);
extern int _aasg_rdma_cancel(struct aasg_peer *aasgp, struct xm_message *xmmsg);
extern ssize_t _aasg_serdes(xdrproc_t xdrproc,
			    void *cooked,
			    void *raw, size_t size,
			    enum xdr_op xop);
extern ssize_t _aasg_pserdes(struct aasg_context *aasgctx,
			     xdrproc_t xdrproc,
			     void *cooked,
			     enum xdr_op xop);
extern void _aasg_ignore(struct xm_transport *xmx, struct xm_message *xmmsgs);
extern int _aasg_request(struct aasg_context *aasgctx,
			 uint32_t proc,
			 void *args, size_t sizargs,
			 void *res, size_t sizres);
#endif
#endif
