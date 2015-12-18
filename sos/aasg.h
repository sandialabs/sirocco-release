#include <time.h>
#include "asg.h"
#include <sxm.h>
#include <ssm.h>
#include <ssmptcp.h>

typedef intptr_t asg_transport_t;

struct aasg_context;

extern int aasg_create_transport(struct xm_transport *xmx,
				 asg_transport_t *transportp);
extern int aasg_destroy_transport(asg_transport_t *transportp);
extern int aasg_create_location(asg_transport_t transport,
				const ssm_Haddr addr,
				asg_location_t *locationp);
extern int aasg_destroy_location(asg_location_t *loc);

extern int aasg_error(struct asg_batch_id_t *ctx);
extern int aasg_return(struct asg_batch_id_t *ctx);
