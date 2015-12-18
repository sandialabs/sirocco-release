#ifndef _IDB_CACHE_H_
#define _IDB_CACHE_H_

#include <idb.h>

/* idb_cache.h: Interface for small IDB caches.
 *
 * Use to enable sharing of lists of IDBs between groups of transactions.
 */

/* Create a new cache */
struct idb_cache *hoss_idb_cache_create(void);

/* Create a new reference to the cache. */
struct idb_cache *hoss_idb_cache_ref(struct idb_cache *cache);

/* Retrieve an entry from the cache. If no entry exists, one will be
 * created and inserted. */
int hoss_idb_cache_get(IDB *idb, const char *path, int rdonly, IDBC c,
		      struct idb_cache *cache);

/* Destroy a reference to a cache. When all known references are
 * destroyed, the resources tied to the list are freed. */
int hoss_idb_cache_deref(struct idb_cache *cache);

#endif
