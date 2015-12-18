#include <stdlib.h>
#include <strings.h>
#include <errno.h>
#include <assert.h>

#include "aasg.h"
#include "aasg_intnl.h"

void
_aasg_set_nsid1(nsid1 *ident,
		asg_container_id_t container,
		asg_object_id_t object,
		asg_fork_id_t fork)
{

	ident->container = container;
	ident->object = object;
	ident->fork = fork;
}

void
_aasg_complete(struct asg_batch_id_t *bid)
{
	int	err;

	do {
		if ((err = bid->msg[0].error)) {
			break;
		}
	} while (0);

	if (bid->setret != NULL) {
		bid->setret(bid->ctx);
	} else {
		bid->error = err;
	}

	if (bid->callback != NULL) {
		bid->callback(bid->cb_data, bid->error);
	} 
}

int
_aasg_setflags(asg_flags_t flags, cond1 *c)
{
	cond1 cond = SOS1_COND_NONE;
	int nf = 0;

	if (flags & ASG_COND_UNTIL) {
		cond = SOS1_COND_UNTIL;
		nf++;
	}
	if (flags & ASG_COND_ALL) {
		cond = SOS1_COND_ALL;
		nf++;
	}

	if (flags & ASG_AUTO_UPDATE_ID) {
		cond = SOS1_UPD_AUTO;
		nf++;
	}

	if (nf > 1)
		return -EINVAL;
	*c = cond;
	return 0;
}
