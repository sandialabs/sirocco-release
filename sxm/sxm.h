/* SXM - An XM-ish wrapper over SSM
 *
 * Differences between XM and SSM:
 *
 * - XM allows for posting buffers to the match entry's pool before
 *   posting the match entry. SSM does not. This wrapper does not
 *   allow preposting buffers into the pool before inserting the match
 *   entry.
 * - There is no xm_listen function, as SSM listens from
 *   initialization.
 * - XM takes a particular format of addresses that is not amenable to
 *   SSM at present. All XM structures will refer to SSM addresses.
 * - xm_wait does not take any arguments, while ssm_wait
 *   does. Applications should call ssm_wait instead of xm_wait.
 * - The interface is a little wrinkly with respect to transport
 *   objects, with an xm_transport containing only an ssm_transport,
 *   which at present is user-initialized. This was done anticipating
 *   a need for an xm_transport type, but that has not yet proven true.
 * - Matching will not honor read- and write-only flags for a target
 *   buffer. Posting a message will set XM_F_IFRW in the message.
 */

#ifndef __SXM_H_
#define __SXM_H_

extern int xm_verbose;

#include <ssm.h>
#include <ssmptcp.h>
#include <stdint.h>
#include <unistd.h>

#define XM_F_IFRDONLY		0x0
#define XM_F_IFWRONLY		0x1
#define XM_F_IFRW		0x2
#define XM_F_IFMT		(XM_F_IFRDONLY|XM_F_IFWRONLY|XM_F_IFRW)

#define XM_F_ISRDONLY(__f)	(((__f) & XM_F_IFMT) == XM_F_IFRDONLY)
#define XM_F_ISWRONLY(__f)	(((__f) & XM_F_IFMT) == XM_F_IFWRONLY)
#define XM_F_ISRW(__f)		(((__f) & XM_F_IFMT) == XM_F_IFRW)

typedef uint64_t xm_match_t;

enum xm_match_where {
	XM_MATCH_HEAD = SSM_POS_HEAD,
	XM_MATCH_TAIL = SSM_POS_TAIL,
	XM_MATCH_BEFORE = SSM_POS_BEFORE,
	XM_MATCH_AFTER = SSM_POS_AFTER,
};

union xm_data {
	void *ptr;
	int i;
	uint32_t u32;
	uint64_t u64;
};

struct xm_transport {
	ssm_id ssm;
	ssm_Itp itp;
	ssm_Iaddr addrs;
};

struct xm_message {
	unsigned flags;
	ssm_Haddr addr;			// snd/rcv addr
	ssm_bits match;			// bits
	const struct iovec *iov;	// buf
	size_t	iovcnt;			// # in iovec
	uint64_t len;
	int	error;
	void (*callback)(struct xm_transport *, struct xm_message *);
	union xm_data data;		// user data

	ssm_cb_t _cb;
	ssm_tx _tx;
	ssm_md _md;
	ssm_mr _mr;
	struct xm_transport *_xmx;
};

struct xm_match_entry;

int xmtcp_start(struct xm_transport **, uint16_t listen_port);
ssm_Haddr xmtcp_addr_create(struct xm_transport *, char *addr, uint16_t port);
void xm_stop(struct xm_transport *);

int xm_match_add_to_pool(struct xm_match_entry *xmme,
			 struct xm_message *xmmsg);
int xm_get(struct xm_transport *xmx, struct xm_message *xmmsg);
int xm_put(struct xm_transport *xmx, struct xm_message *xmmsg);
int xm_send(struct xm_transport *xmx, struct xm_message *xmmsg);
int xm_match_remove(struct xm_match_entry *xmme);
int xm_match_insert(struct xm_match_entry *xmme, enum xm_match_where where,
			    struct xm_match_entry *anchor);
int xm_match_remove_from_pool(struct xm_match_entry *xmme,
			      struct xm_message *xmmsg);
void xm_match_destroy(struct xm_match_entry*xmme);
int xm_match_create(struct xm_transport *xmx, xm_match_t match,
			    xm_match_t mask, struct xm_match_entry **xmmep);

#endif
