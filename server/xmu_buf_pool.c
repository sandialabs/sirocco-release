#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>

#include <sxm.h>
#include "xmu_private.h"
#include "xmu_obj_pools.h"

struct xmu_buf_pool_entry {
	struct iovec iov;
	struct xm_message m;
	TAILQ_ENTRY(xmu_buf_pool_entry) entries;
};

int
xmu_buf_pool_construct(
	struct xm_match_entry *me,
	struct xmu_buf_pool *p,
	size_t npes,
	size_t bufsize,
	void (*callback)(struct xm_transport *, struct xm_message *),
	void *data,
	size_t *nonline)
{
	int rc = 0;
	struct xmu_buf_pool_entry *pe;
	size_t len;

	p->me = me;
	TAILQ_INIT(&p->bs);
	if (pthread_mutex_init(&p->m, NULL))
		abort();

	for (*nonline = 0; *nonline < npes; (*nonline)++) {
		len = sizeof(*pe) + bufsize;
		pe = malloc(len);
		if (pe == NULL && (rc = -errno)) {
			ERR("Could not malloc buf (%lub): %s\n",
			    len, strerror(-rc));
			break;
		}

		pe->iov.iov_base = (char *)(pe + 1);
		pe->iov.iov_len = bufsize;

		pe->m.flags = XM_F_IFRW;
		pe->m.addr = NULL;
		pe->m.match = 0;
		pe->m.iov = &pe->iov;
		pe->m.iovcnt = 1;
		pe->m.callback = callback;
		pe->m.data.ptr = data;

		rc = xm_match_add_to_pool(me, &pe->m);
		if (rc != 0) {
			WARN("Could not insert match entry "
			     "%lu: %s\n", *nonline, strerror(-rc));
			break;
		}
		TAILQ_INSERT_HEAD(&p->bs, pe, entries);
	}
	if (*nonline < npes) {
		free(pe);
	}
	p->nonline = *nonline;
	return rc;
}

int
xmu_buf_pool_destroy(
	struct xmu_buf_pool *p,
	size_t *nonline)
{
	int rc = 0;
	struct xmu_buf_pool_entry *pe;

	*nonline = p->nonline;
	while(*nonline > 0) {
		pe = p->bs.tqh_first;
		rc = xm_match_remove_from_pool(p->me, &pe->m);
		if (rc != 0) {
			break;
		}
		TAILQ_REMOVE(&p->bs, pe, entries);
		free(pe);
		(*nonline)--;
	}
	p->nonline = *nonline;
	return rc;
}
