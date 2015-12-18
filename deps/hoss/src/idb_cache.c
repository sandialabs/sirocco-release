#include <errno.h>
#include <idb.h>
#include <stdio.h>
#include <stdlib.h>

#include "idb_cache.h"

/* Simple list-based implementation, which should be fine for small
 * groups/subgroups. */

struct idb_cache {
	IDB *idbs;
	size_t n;
	size_t max;
	size_t refcount;
};

struct idb_cache *
hoss_idb_cache_create(void)
{
	struct idb_cache *ret;

	ret = malloc(sizeof(*ret));
	if (ret != NULL) {
		ret->idbs = NULL;
		ret->n = 0;
		ret->max = 0;
		ret->refcount = 1;
	}
	return ret;
}

struct idb_cache *
hoss_idb_cache_ref(struct idb_cache *cache)
{
	cache->refcount++;
	return cache;
}

int
hoss_idb_cache_get(IDB *idb, const char *path, int rdonly, IDBC c,
		 struct idb_cache *cache)
{
	IDB *tmp;
	size_t newmax;

	if (cache->max == cache->n) {
		newmax = cache->max * 2 + 1;
		tmp = realloc(cache->idbs, newmax * sizeof(IDB));
		if (tmp != NULL) {
			cache->max = newmax;
			cache->idbs = tmp;
		} else {
			return ENOMEM;
		}
	}

	tmp = cache->idbs + cache->n;
	(*tmp) = idb_open(path, rdonly, c);
	if ((*tmp) == NULL)
		return errno;
	*idb = *tmp;
	cache->n += 1;
	return 0;
}

int
hoss_idb_cache_deref(struct idb_cache *cache)
{
	size_t i;
	int rc;

	if (cache->refcount == 1) {
		for (i = 0; i < cache->n; i++) {
			if (cache->idbs[i] != NULL) {
				rc = idb_close(cache->idbs[i]);
				if (rc)
					return rc;
				cache->idbs[i] = NULL;
			}
		}
		free(cache->idbs);
		free(cache);
	} else {
		cache->refcount--;
	}

	return 0;
}
