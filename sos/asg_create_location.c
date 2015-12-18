#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#include "aasg.h"
#include "aasg_intnl.h"

int
aasg_create_location(asg_transport_t transport,
		     const ssm_Haddr addr,
		     asg_location_t *locationp)
{
	struct aasg_transport *aasgx;
	struct aasg_peer *aasgp;

	aasgx = (struct aasg_transport *)transport;
	if (aasgx == NULL || addr == NULL)
		return -EINVAL;

	aasgp = malloc(sizeof(struct aasg_peer));
	if (aasgp == NULL)
		return -ENOMEM;
	aasgp->aasgx = aasgx;
	aasgp->addr = ssm_addr_cp(aasgx->xmx->ssm, addr);
	*locationp = (intptr_t )aasgp;
	return 0;
}

int
aasg_destroy_location(asg_location_t *loc)
{
	struct aasg_peer *aasgp;

	aasgp = (struct aasg_peer *)*loc;

	free(aasgp->addr);
	free(aasgp);

	return 0;
}
