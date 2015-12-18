#ifndef _XM_OBJ_POOLS_H_
#define _XM_OBJ_POOLS_H_

#include <pthread.h>
#include <stdint.h>
#include <sys/queue.h>

struct xmu_buf_pool;
struct xmu_me_pool;
struct xmu_me_pool_entry {
	xm_match_t match;
	struct xm_match_entry *me;
	TAILQ_ENTRY(xmu_me_pool_entry) entries;
};

int
xmu_buf_pool_construct(
	struct xm_match_entry *me,
	struct xmu_buf_pool *pool,
	size_t  npes,
	size_t bufsize,
	void (*callback)(struct xm_transport *, struct xm_message *),
	void *data,
	size_t *nonline);

int
xmu_buf_pool_destroy(
	struct xmu_buf_pool *p,
	size_t *nonline);

int
xmu_me_pool_init(struct xmu_me_pool *p, struct xm_transport *xmx,
		 xm_match_t base, size_t nentries, size_t *nonline);

int
xmu_me_pool_add(struct xmu_me_pool *p, size_t nentries, size_t *nonline);

struct xmu_me_pool_entry *
xmu_me_pool_get(struct xmu_me_pool *p);

void
xmu_me_pool_put(struct xmu_me_pool_entry *mpe, struct xmu_me_pool *p);

int
xmu_me_pool_destroy(struct xmu_me_pool *p);

/* NO PEAKING. */
struct xmu_buf_pool {
	TAILQ_HEAD(xmu_bph, xmu_buf_pool_entry) bs;
	size_t nonline;
	struct xm_match_entry *me;
	pthread_mutex_t m;
};

struct xmu_me_pool {
	TAILQ_HEAD(xmu_mph, xmu_me_pool_entry) mes;
	xm_match_t base;
	size_t nentries;
	pthread_mutex_t m;
	struct xm_transport *xmx;
};
#endif
