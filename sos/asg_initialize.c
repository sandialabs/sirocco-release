#include <stdlib.h>
#include <strings.h>
#include <errno.h>
#include <assert.h>

#include "aasg.h"
#include "aasg_intnl.h"

int
asg_initialize(asg_instance_t *instance,
	       const char *options __attribute__ ((unused)))
{
	size_t i;
	asg_instance *inst = malloc(sizeof(asg_instance));
	if (inst == NULL)
		return -ENOMEM;
	for (i = 0; i < AASG_OPS; i++) {
		inst->handler[i] = NULL;
	}
	*instance = (asg_instance_t) inst;

	return 0;
}
