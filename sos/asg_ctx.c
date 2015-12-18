#include <stdlib.h>
#include <strings.h>
#include <errno.h>
#include <assert.h>

#include "aasg.h"
#include "aasg_intnl.h"

int
_aasg_bidalloc(struct aasg_peer *aasgp,
	       void (*completion_handler)(struct asg_batch_id_t *),
	       struct asg_batch_id_t **bid)
{
	int id;
	struct aasg_transport *aasgx;

	aasgx = aasgp->aasgx;

	*bid = malloc(sizeof(struct asg_batch_id_t));	
	if (*bid == NULL)
		return -ENOMEM;

	memset(*bid, 0, sizeof(struct asg_batch_id_t));

	(*bid)->ctx = malloc(sizeof(struct context));
	memset((*bid)->ctx, 0, sizeof(struct context));
	if ((*bid)->ctx == NULL) {
		free(*bid);
		return -ENOMEM;
	}

	id = 0;
	pthread_mutex_lock(&aasgx->mp);
	while (aasgx->map[id] == 1 && id < AASG_NGRP) {
		id++;
	}
	if (id < AASG_NGRP) {
		aasgx->map[id] = 1;
	}
	pthread_mutex_unlock(&aasgx->mp);

	if (id == AASG_NGRP) {
		free((*bid)->ctx);
		free(*bid);
		return -ENOBUFS;
	}

	(*bid)->aasgp = aasgp;
	(*bid)->id = id;
	(*bid)->completion_handler = completion_handler;
	(*bid)->error = -EINPROGRESS;
	(*bid)->setret = NULL;
	(*bid)->ctx->bid = *bid;
	(*bid)->cmdbuf = NULL;
	(*bid)->mbitc = 0;
	(*bid)->len = 0;
	return 0;
}

void
_aasg_bidfree(struct asg_batch_id_t *bid)
{
	struct aasg_transport *aasgx;

	aasgx = bid->aasgp->aasgx;

	pthread_mutex_lock(&aasgx->mp);
	aasgx->map[bid->id] = 0;
	pthread_mutex_unlock(&aasgx->mp);

}
