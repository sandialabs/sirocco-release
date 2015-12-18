#include <stdlib.h>
#include <strings.h>
#include <errno.h>
#include <assert.h>

#include "aasg.h"
#include "aasg_intnl.h"

static int
error_accessor(struct asg_batch_id_t *ctx)
{
	return ctx->error;
}

int
aasg_error(struct asg_batch_id_t *ctx)
{
	//(void )ssm_wait(ctx->aasgp->aasgx->xmx->ssm, 0);
	return error_accessor(ctx);
}

int
aasg_return(struct asg_batch_id_t *ctx)
{
	int	err;

	if (ctx == NULL) {
		return -1;
	}

	while ((err = aasg_error(ctx)) == -EINPROGRESS) {}

	return err;
}
