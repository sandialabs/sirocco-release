#include <assert.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <ssm.h>
#include <ssmptcp.h>
#include <unistd.h>

#include "sxm.h"

int xm_verbose;
int xm_flaky_put;
int xm_flaky_get;

int failed_gets = 0;
int failed_puts = 0;
int total_gets = 0;
int total_puts = 0;

static void
minit(pthread_mutex_t *m)
{
	int rc;

	if ((rc = pthread_mutex_init(m, NULL)))
		abort();
}

static void
mlk(pthread_mutex_t *m)
{
	int rc;

	if ((rc = pthread_mutex_lock(m)))
		abort();
}

static void
munlk(pthread_mutex_t *m)
{
	int rc;

	if ((rc = pthread_mutex_unlock(m)))
		abort();
}

static void
print_ssm_event(void *cbdat, void *evdat)
{
	ssm_result r = evdat;
	(void)cbdat;
	fprintf(stderr, "cbdat             = %p\n", cbdat);
	fprintf(stderr, "ssm_id id         = %p\n", r->id);
	fprintf(stderr, "ssm_me me         = %p\n", r->me);
	fprintf(stderr, "ssm_tx tx         = %p\n", r->tx);
	fprintf(stderr, "ssm_bits bits     = 0x%lx\n", r->bits);
	fprintf(stderr, "ssm_status status = %u (%s)\n", r->status,
		ssm_status_str(r->status));
	fprintf(stderr, "                    (%s)\n", ssm_op_str(r->op));
	fprintf(stderr, "ssm_Haddr addr    = %p\n", r->addr);
	fprintf(stderr, "ssm_mr mr         = %p\n", r->mr);
	fprintf(stderr, "ssm_md md         = %p\n", r->md);
	fprintf(stderr, "uint64_t bytes    = %lu\n", r->bytes);
}

static int
ssm_to_errno(int ssm_err)
{
	switch(ssm_err) {
	case SSM_ST_NOMATCH:
		return ENOENT;
	case SSM_ST_NOBUF:
		return ENOBUFS;
	case SSM_ST_NETFAIL:
		return ENOLINK;
	case SSM_ST_REMOTE:
		return EIO;
	case SSM_ST_DROP:
	case SSM_ST_CANCEL:
		return ECANCELED;
	case SSM_ST_COMPLETE:
		return 0;
	}
	return ssm_err;
}

/* For callbacks on puts and gets */
static void
xm_ssm_send_wrap(void *cb_data, void *ev_data)
{
	struct xm_message *xmmsg = cb_data;
	ssm_result r = ev_data;

	if (xm_verbose >= 2)
		print_ssm_event(cb_data, ev_data);
	r->me = NULL;
	/* Both have unconditional success returns */
	(void)ssm_mr_destroy(xmmsg->_mr);
	(void)ssm_md_release(xmmsg->_md);

	/* Best way to convert ssm_result status to errno */
	xmmsg->error = ssm_to_errno(r->status);
	xmmsg->callback(xmmsg->_xmx, xmmsg);
}

/* Needed to map from MR to xm message for callback */
struct msg_and_mr {
	struct xm_message *xmmsg;
	ssm_mr mr;
};

struct xm_match_entry {
	xm_match_t match;
	xm_match_t mask;
	struct xm_transport *xmx;
	ssm_me me;
	unsigned posted:1;

	size_t nlinked;

	struct msg_and_mr *maps;
	size_t nmaps;
	size_t maxmaps;

	ssm_cb_t cb; /* Used to chain to msg-specific cb */
	pthread_mutex_t m; /* Protects maps and nlinked */
};

static size_t
get_map_idx(struct xm_match_entry *xmme, ssm_mr mr)
{
	size_t i;
	for (i = 0; i < xmme->nmaps; i++) {
		if (xmme->maps[i].mr == mr)
			break;
	}
	return i;
}

static void
remove_map_idx(struct xm_match_entry *xmme, size_t i) {
	xmme->nmaps--;
	xmme->maps[i] = xmme->maps[xmme->nmaps];
}

/* For callbacks on matches */
static void
xm_ssm_match_wrap(void *cb_data, void *ev_data)
{
	struct xm_match_entry *xmme = cb_data;
	ssm_result r = ev_data;
	struct xm_message *xmmsg;
	size_t i;

	mlk(&xmme->m);
	i = get_map_idx(xmme, r->mr);
	if (i <= xmme->nmaps && xmme->nmaps > 0) {
		if (xm_verbose >= 2)
			print_ssm_event(cb_data, ev_data);
		xmmsg = xmme->maps[i].xmmsg;
		remove_map_idx(xmme, i);
		munlk(&xmme->m);
		/* No very good way to preserve the MR/MD across callback */
		(void)ssm_mr_destroy(xmmsg->_mr);
		(void)ssm_md_release(xmmsg->_md);

		xmmsg->match = r->bits;
		xmmsg->len = r->bytes;
		xmmsg->error = ssm_to_errno(r->status);
		xmmsg->addr = r->addr;
		xmmsg->callback(xmmsg->_xmx, xmmsg);
		xmmsg->addr = NULL;
	} else {
		fprintf(stderr, "ERROR: Could not match SSM message to XM"
			"message\n");
		print_ssm_event(cb_data, ev_data);
		fprintf(stderr, "%lu maps\n", xmme->nmaps);
		for (i = 0; i < xmme->nmaps; i++) {
			fprintf(stderr, "ssm_mr: %p\txm_msg:%p\n",
				xmme->maps[i].mr, xmme->maps[i].xmmsg);
		}
		munlk(&xmme->m);
	}
}

int
xm_match_create(struct xm_transport *xmx, xm_match_t match, xm_match_t mask,
		struct xm_match_entry **xmmep)
{
	struct xm_match_entry *xmme;
	xmme = *xmmep = malloc(sizeof(*xmme));
	if (xmme == NULL)
		return -errno;

	xmme->cb.pcb = xm_ssm_match_wrap;
	xmme->cb.cbdata = xmme;
	xmme->match = match;
	xmme->mask = mask;
	xmme->xmx = xmx;
	minit(&xmme->m);
	mlk(&xmme->m);
	xmme->nlinked = 0;
	xmme->nmaps = 0;
	xmme->maxmaps = 0;
	xmme->maps = NULL;
	munlk(&xmme->m);
	return 0;
}

void
xm_match_destroy(struct xm_match_entry*xmme)
{
	mlk(&xmme->m);
	free(xmme->maps);
	xmme->maps = NULL;
	munlk(&xmme->m);
	free(xmme);
}

int
xm_match_insert(struct xm_match_entry *xmme, enum xm_match_where where,
		struct xm_match_entry *anchor)
{
	xmme->me = ssm_link(xmme->xmx->ssm, xmme->match, xmme->mask, where,
			    (anchor == NULL) ? NULL : anchor->me,
			    &xmme->cb, 0);
	if (xmme->me != NULL) {
		xmme->posted = 1;
		return 0;
	}
	return -errno;
}

int
xm_match_remove(struct xm_match_entry *xmme)
{
	int rc;

	if (xmme->nlinked) {
		errno = ENOTEMPTY;
		return -errno;
	}
	if (xmme->posted) {
		rc = ssm_unlink(xmme->xmx->ssm, xmme->me);
		if (rc) {
			errno = ssm_to_errno(rc);
			return -errno;
		}
		xmme->posted = 0;
	}
	return 0;
}

#define STASH_ERRNO(v) (((v) = errno) || 1)

int
xm_send(struct xm_transport *xmx, struct xm_message *xmmsg)
{
	int err;
	size_t acc = 0, i;

	if (XM_F_ISRW(xmmsg->flags))
		return -(errno = EINVAL);
	assert(XM_F_ISRDONLY(xmmsg->flags) || XM_F_ISWRONLY(xmmsg->flags));
	if (xm_flaky_get && XM_F_ISWRONLY(xmmsg->flags) &&
	    rand() % 100 <= xm_flaky_get) {
		xmmsg->flags = 0xDEADBEEF;
		failed_gets++;
		fprintf(stderr, "!!!WARNING!!! "
			"Flaky get is about to return ENETFAIL (%i%% so far)\n",
			failed_gets * 100 / (failed_gets + total_gets));
	}
	if (xm_flaky_put && XM_F_ISRDONLY(xmmsg->flags) &&
	    rand() % 100 <= xm_flaky_put) {
		xmmsg->flags = 0xDEADBEEF;
		failed_puts++;
		fprintf(stderr, "!!!WARNING!!! "
			"Flaky put is about to return ENETFAIL (%i%% so far)\n",
			failed_puts * 100 / (failed_puts + total_puts));
		
	}

	xmmsg->_cb.pcb = xm_ssm_send_wrap;
	xmmsg->_cb.cbdata = xmmsg;
	xmmsg->_xmx = xmx;

	xmmsg->_md = NULL;

	for (i = 0; i < xmmsg->iovcnt; i++)
		acc += xmmsg->iov[i].iov_len;
	xmmsg->_md = ssm_md_addv(xmmsg->_md, (struct iovec *)xmmsg->iov,
				 xmmsg->iovcnt);
	if (xmmsg->_md == NULL && STASH_ERRNO(err))
		goto md_failed;

	xmmsg->_mr = ssm_mr_create(xmmsg->_md, 0, acc);
	if (xmmsg->_mr == NULL && STASH_ERRNO(err))
		goto mr_failed;

	if (XM_F_ISRDONLY(xmmsg->flags)) {
		xmmsg->_tx = ssm_put(xmx->ssm, xmmsg->addr, xmmsg->_mr, NULL,
				     xmmsg->match, &xmmsg->_cb, SSM_NOF);
		total_puts++;
	} else if (XM_F_ISWRONLY(xmmsg->flags)) {
		xmmsg->_tx = ssm_get(xmx->ssm, xmmsg->addr, xmmsg->_md, xmmsg->_mr,
				     xmmsg->match, &xmmsg->_cb, SSM_NOF);
		total_gets++;
	} else {
		/* Flaked, or a serious error */
		assert(xmmsg->flags == 0xDEADBEEF);
		ssm_result_t res = {
			.id = xmx->ssm,
			.me = NULL,
			.tx = xmmsg->_tx,
			.bits = xmmsg->match,
			.status = SSM_ST_DROP,
			.op = 0,
			.addr = xmmsg->addr,
			.mr = xmmsg->_mr,
			.md = xmmsg->_md,
			.bytes = 0,
		};
		xm_ssm_send_wrap(xmmsg, &res);
		return 0;
	}

	if (xmmsg->_tx == NULL && STASH_ERRNO(err))
		goto op_failed;
	return 0;

 op_failed:
	(void)ssm_mr_destroy(xmmsg->_mr);
 mr_failed:
	(void)ssm_md_release(xmmsg->_md);
 md_failed:
	return -(errno = err);
}

int
xm_put(struct xm_transport *xmx, struct xm_message *xmmsg)
{
	xmmsg->flags = (xmmsg->flags & (~XM_F_IFMT)) | XM_F_IFRDONLY;
	return xm_send(xmx, xmmsg);
}

int
xm_get(struct xm_transport *xmx, struct xm_message *xmmsg)
{
	xmmsg->flags = (xmmsg->flags & (~XM_F_IFMT)) | XM_F_IFWRONLY;
	return xm_send(xmx, xmmsg);
}

int
xm_match_add_to_pool(struct xm_match_entry *xmme, struct xm_message *xmmsg)
{
	int rc, err;
	size_t tmp;
	void *vtmp;
	struct msg_and_mr *mar;
	size_t acc = 0, i;

	xmmsg->flags = (xmmsg->flags & (~XM_F_IFMT)) | XM_F_IFRW;

	xmmsg->_md = NULL;
	xmmsg->_md = ssm_md_addv(xmmsg->_md, (struct iovec *)xmmsg->iov,
				 xmmsg->iovcnt);
	if (xmmsg->_md == NULL && STASH_ERRNO(err))
		goto md_failed;

	for (i = 0; i < xmmsg->iovcnt; i++)
		acc += xmmsg->iov[i].iov_len;
	xmmsg->_mr = ssm_mr_create(xmmsg->_md, 0, acc);
	if (xmmsg->_mr == NULL && STASH_ERRNO(err))
		goto mr_failed;

	xmmsg->_xmx = xmme->xmx;


	mlk(&xmme->m);
	if (xmme->maxmaps == xmme->nmaps) {
		tmp = xmme->maxmaps * 2 + 1;
		vtmp = realloc(xmme->maps, tmp * sizeof(struct msg_and_mr));
		if (vtmp == NULL && STASH_ERRNO(err)) {
			goto realloc_failed;
		}
		xmme->maps = vtmp;
		xmme->maxmaps = tmp;
	}
	mar = xmme->maps + xmme->nmaps;
	mar->mr = xmmsg->_mr;
	mar->xmmsg = xmmsg;
	xmme->nmaps++;
	munlk(&xmme->m);

	rc = ssm_post(xmme->xmx->ssm,
		      xmme->me,
		      xmmsg->_mr,
		      SSM_NOF);
	if (rc != 0 && (err = ssm_to_errno(rc))) {
		goto post_failed;
	}

	return 0;
 post_failed:
	mlk(&xmme->m);
	tmp = get_map_idx(xmme, xmmsg->_mr);
	if (tmp < xmme->nmaps)
		remove_map_idx(xmme, tmp);
	munlk(&xmme->m);
	/* Optional: Shrink array again */
 realloc_failed:
	(void)ssm_mr_destroy(xmmsg->_mr);
 mr_failed:
	(void)ssm_md_release(xmmsg->_md);
 md_failed:
	return -(errno = err);
}

int
xm_match_remove_from_pool(struct xm_match_entry *xmme,
			   struct xm_message *xmmsg)
{
	int rc;
	size_t tmp;

	rc = ssm_drop(xmme->xmx->ssm, xmme->me, xmmsg->_mr);
	if (rc != 0) {
		rc = ssm_to_errno(rc);
		return -(errno = rc);
	}

	mlk(&xmme->m);
	tmp = get_map_idx(xmme, xmmsg->_mr);
	if (tmp < xmme->nmaps)
		remove_map_idx(xmme, tmp);
	munlk(&xmme->m);

	(void)ssm_mr_destroy(xmmsg->_mr);
	(void)ssm_md_release(xmmsg->_md);
	return 0;
}

ssm_Haddr
xmtcp_addr_create(struct xm_transport *xmx, char *addr, uint16_t port)
{
	ssmptcp_addrargs_t addrargs = {
		.host = addr,
		.port = port,
	};
	return xmx->addrs->create(xmx->addrs, &addrargs);
}

int
xmtcp_start(struct xm_transport **xmxp, uint16_t listen_port)
{
	struct xm_transport *xmx;
	int err;

	if (xm_flaky_get || xm_flaky_put) {
		fprintf(stderr, "!!! WARNING !!!\n");
		fprintf(stderr, "!!! xm_flakey is set (%i%% on puts, "
			"%i%% on gets), some transfers will fail !!!\n",
			((xm_flaky_put <= 100) ? xm_flaky_put : 100),
			((xm_flaky_get <= 100) ? xm_flaky_get : 100));
	}

	xmx = *xmxp = malloc(sizeof(*xmx));
	if (xmx == NULL && STASH_ERRNO(err))
		goto xmx_failed;

	xmx->itp = ssmptcp_new_tp(listen_port, SSM_NOF);
	if (xmx->itp == NULL && STASH_ERRNO(err))
		goto tp_failed;

	xmx->ssm = ssm_start(xmx->itp, NULL, SSM_NOF);
	if (xmx->ssm == NULL && STASH_ERRNO(err))
		goto start_failed;

	xmx->addrs = ssm_addr(xmx->ssm);
	if (xmx->addrs == NULL && STASH_ERRNO(err))
		goto addr_failed;

	return 0;

 addr_failed:
	(void)ssm_stop(xmx->ssm);
 start_failed:
	free(xmx->itp);
 tp_failed:
	free(xmx);
 xmx_failed:
	return -(errno = err);
}

void xm_stop(struct xm_transport *xmx) {
	ssm_stop(xmx->ssm);
	free(xmx->itp);
	free(xmx->addrs);
	free(xmx);
}
