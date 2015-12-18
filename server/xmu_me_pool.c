#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>

#include "asg_private.h"
#include "Xm.h"
#include "xmu_obj_pools.h"

int
xmu_me_pool_init(struct xmu_me_pool *p, struct xm_transport *xmx,
		 xm_match_t base, size_t nentries, size_t *nonline)
{
	int rc;

	p->base = base;
	p->nentries = 0;
	p->xmx = xmx;
	rc = pthread_mutex_init(&p->m, NULL);
	if (rc != 0)
		return -rc;
	TAILQ_INIT(&p->mes);

	rc = xmu_me_pool_add(p, nentries, nonline);
	return rc;
}

int
xmu_me_pool_add(struct xmu_me_pool *p, size_t nentries, size_t *nonline)
{
	int rc;
	struct xmu_me_pool_entry *mpe;
	size_t i;

	if (pthread_mutex_lock(&p->m))
		abort();

	for (i = 0; i < nentries; i++) {
		mpe = malloc(sizeof(*mpe));
		if (mpe == NULL && (rc = errno))
			break;
		mpe->match = p->base + p->nentries;
		rc = Xm_match_create(p->xmx, mpe->match, 0, &mpe->me);
		if (rc != 0) {
			ERR("Could not create match entry: %s\n",
			    strerror(-rc));
			free(mpe);
			break;
		}
		rc = Xm_match_insert(mpe->me, XM_MATCH_TAIL, NULL);
		if (rc != 0) {
			ERR("Could not insert match entry: %s\n",
			    strerror(-rc));
			free(mpe);
			/* Leak me */
			break;
		}
		TAILQ_INSERT_HEAD(&p->mes, mpe, entries);
		p->nentries++;
	}
	*nonline = p->nentries;;
	if (pthread_mutex_unlock(&p->m))
		abort();
	return rc;
}

struct xmu_me_pool_entry *
xmu_me_pool_get(struct xmu_me_pool *p)
{
	struct xmu_me_pool_entry *mpe;

	if (pthread_mutex_lock(&p->m))
		abort();

	mpe = p->mes.tqh_first;
	if (mpe != NULL)
		TAILQ_REMOVE(&p->mes, mpe, entries);

	if (pthread_mutex_unlock(&p->m))
		abort();
	return mpe;
}

void
xmu_me_pool_put(struct xmu_me_pool_entry *mpe, struct xmu_me_pool *p)
{
	if (pthread_mutex_lock(&p->m))
		abort();

	if (mpe != NULL)
		TAILQ_INSERT_HEAD(&p->mes, mpe, entries);

	if (pthread_mutex_unlock(&p->m))
		abort();
}

int
xmu_me_pool_destroy(struct xmu_me_pool *p)
{
	int rc = 0;
	struct xmu_me_pool_entry *mpe;

	if (pthread_mutex_lock(&p->m))
		abort();

	while (p->nentries) {
		mpe = p->mes.tqh_first;
		if (mpe == NULL) {
			rc = -EAGAIN;
		}
		rc = Xm_match_remove(mpe->me);
		if (rc != 0) {
			WARN("Xm_match_remove failed: %s\n",
			     strerror(-rc));
			break;
		}
		TAILQ_REMOVE(&p->mes, mpe, entries);
		/* Leak match entry */
		free(mpe);
		p->nentries--;
	}
	assert(p->nentries || p->mes.tqh_first == NULL);

	if (pthread_mutex_unlock(&p->m))
		abort();
	return rc;
}
