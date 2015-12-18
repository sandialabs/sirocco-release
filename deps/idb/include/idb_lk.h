#ifndef __IDB_LK_H__
#define __IDB_LK_H__

static void
mlk(pthread_mutex_t *m)
{
	int rc;

	rc = pthread_mutex_lock(m);
	if(rc != 0) {
		assert(0);
		abort();
	}
}

static void
munlk(pthread_mutex_t *m)
{
	int rc;

	rc = pthread_mutex_unlock(m);
	if(rc != 0) {
		assert(0);
		abort();
	}
}

static void
csig(pthread_cond_t *c)
{
	int rc;

	rc = pthread_cond_signal(c);
	if (rc != 0) {
		assert(0);
		abort();
	}
}

#endif
