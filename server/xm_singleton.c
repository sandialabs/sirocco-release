#include <assert.h>
#include <logging.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sxm.h>

#include "xm_singleton.h"

#define USE_CRITICAL_SECTION 1

int wp_inited = 0;
struct xms_wait_params {
	pthread_t waiter;
	unsigned shutdown:1;
} wp;

static void *
xms_wait(void *arg)
{
	struct xm_transport *xmx = arg;
	struct timeval time = {
		.tv_sec = 1,
		.tv_usec = 0,
	};
	while(!wp.shutdown) {
		ssm_wait(xmx->ssm, &time);
	}
	pthread_exit(NULL);
	return NULL;
}

int
xms_start(struct xm_transport *xmx)
{
	int rc;

	while(!(wp_inited++)) {
		wp.shutdown = 0;
		rc = pthread_create(&wp.waiter,
				    NULL,
				    xms_wait,
				    xmx);
		if (rc != 0) {
			rc = -rc;
			break;
		}
		return 0;
	};
	wp_inited--;
	return rc;
}

int
xms_stop(void)
{
	int rc;

	assert(!wp.shutdown);
	wp.shutdown = 1;
	rc = pthread_join(wp.waiter, NULL);
	if (rc != 0) {
		WARN("Could nto join xm_singleton waiter thread: %s\n",
		     strerror(rc));
		return -rc;
	}
	return 0;
}

void
xms_getl(const char *filename, int line)
{
	INFO(2, "xms_get occurred from %s: %i\n", filename, line);
}

void
xms_putl(const char *filename, int line)
{
	INFO(2, "xms_put occurred from %s: %i\n", filename, line);
	return;
}
