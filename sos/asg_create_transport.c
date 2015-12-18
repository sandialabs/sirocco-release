#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>

#include "aasg.h"
#include "aasg_intnl.h"

static void
*xm_waiter(void *data __attribute__ ((unused))) {
	struct aasg_transport *t = (struct aasg_transport *)data;
	struct timeval w;

	w.tv_sec = 1;
	w.tv_usec = 0;
	pthread_mutex_lock(&t->m);

	while (t->shutdown != 0) {
		pthread_mutex_unlock(&t->m);
		(void)ssm_wait(t->xmx->ssm, &w);
		pthread_mutex_lock(&t->m);
	}

	pthread_cond_signal(&t->sig);
	pthread_mutex_unlock(&t->m);

	return NULL;
}

static int
dgrp(struct xm_match_entry *g[AASG_CALLS_MAX][AASG_DEPTH]) {
	unsigned u, v;
	int err;
	
	for (u = 0; u < AASG_CALLS_MAX; u++) {
		for (v = 0; v < AASG_DEPTH; v++) {
			err = xm_match_remove(g[u][v]);
			if (err)
				break;
			xm_match_destroy(g[u][v]);
		}
	}
	return 0;
}

static int
mkgrp(struct xm_transport *xmx,
      uint8_t mapid,
      struct xm_match_entry *g[AASG_CALLS_MAX][AASG_DEPTH])
{
	unsigned u, v, w, x;
	int	err;

	for (u = 0; u < AASG_CALLS_MAX; u++) {
		for (v = 0; v < AASG_DEPTH; v++) {
			err =
				xm_match_create(xmx,
						MBITC(mapid, u),
						0,
						&g[u][v]);
			if (err)
				break;
			err = xm_match_insert(g[u][v], XM_MATCH_TAIL, NULL);
			if (err) {
				for (w = 0; w < u; w++) {
					for (x = 0; x < AASG_DEPTH; x++) {
						xm_match_remove(g[w][x]);
						xm_match_destroy(g[w][x]);
					}
				}
				break;
			}
		}
	}

	return err;
}

int
aasg_create_transport(struct xm_transport *xmx, asg_transport_t *transportp)
{
	struct aasg_transport *aasgx;
	unsigned u;
	size_t i;
	int err;

	aasgx = malloc(sizeof(struct aasg_transport));
	if (aasgx == NULL)
		return -ENOMEM;
	aasgx->xmx = xmx;
	aasgx->shutdown = 1;
	for (u = 0; u < AASG_NGRP; u++)
		if ((err = mkgrp(aasgx->xmx, u, aasgx->grp[u])))
			break;
	err = pthread_mutex_init(&aasgx->m, NULL);
	if (err) {
		free(aasgx);
		return err;
	}
	err = pthread_cond_init(&aasgx->sig, NULL);
	if (err) {
		free(aasgx);
		return err;
	}
	err = pthread_mutex_init(&aasgx->mp, NULL);
	if (err) {
		free(aasgx);
		return err;
	}

	for (i = 0; i < AASG_NGRP; i++) {
		aasgx->map[i] = 0;
	}
	
	/*
	 * starting an xm_wait() thread here.
	 */

	err = pthread_create(&aasgx->waiter, NULL, &xm_waiter, 
			     (void *) aasgx);

	if (err)
		free(aasgx);
	else
		*transportp = (asg_transport_t )aasgx;
	return err;
}

int 
aasg_destroy_transport(asg_transport_t *transportp) {
	struct aasg_transport *aasgx;
	unsigned u;
	
	aasgx = (struct aasg_transport *)*transportp;

	pthread_mutex_lock(&aasgx->m);
	aasgx->shutdown = 0;
	pthread_cond_wait(&aasgx->sig, &aasgx->m);

	pthread_mutex_unlock(&aasgx->m);
	pthread_join(aasgx->waiter, NULL);

	for (u = 0; u < AASG_NGRP; u++) {
		dgrp(aasgx->grp[u]);
	}

	free(aasgx);

	return 0;
}
