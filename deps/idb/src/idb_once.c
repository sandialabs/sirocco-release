#include <db.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <db.h>
#include "idb.h"
#include "idb_internal.h"
#include "idb_lk.h"
#include "idb_dbg.h"
#ifdef USE_AESOP
#include <aesop/aebdb.hae>
#endif

int _idb_dbg_ddlk = 0;

static int open_env(IDBC idbc, const char *envpath);
static void close_env(IDBC c);
static int open_db(const char *dbname, int rdonly, DB **bdbp, IDBC c,
		   int use_int_cmp);
static void close_db(DB *bdb);

static int
get_time(struct timespec *ts)
{
	int rc;

	struct timeval tv;
	rc = gettimeofday(&tv, NULL);
	if (rc != 0) {
		WARN("gettimeofday gave errno=%i\n", rc);
		return rc;
	}
	ts->tv_sec = tv.tv_sec;
	ts->tv_nsec = tv.tv_usec;
	ts->tv_nsec *= 1000;
	return rc;
}

#if DB_VERSION_MAJOR == 6
static int
idb_comp(DB *db, const DBT *va, const DBT *vb, size_t *locp)
#else
static int
idb_comp(DB *db, const DBT *va, const DBT *vb)
#endif
{
	int rc;
	idb_off_t *a, *b;
	size_t i;

#if DB_VERSION_MAJOR == 6
	(void)locp;
#endif
	(void)db;

	a = va->data;
	b = vb->data;

	rc = 0;
	for (i = 0; i < va->size / sizeof(uint64_t) && rc == 0; i++) {
		if (a[i] < b[i])
			rc = -1;
		else if (a[i] > b[i])
			rc = 1;
	}

	return rc;
}

static void *
checkpointer(void *arg)
{
	int rc;
	struct timespec ts;
	char **begin, **list;
	IDBC idbc;

	idbc = arg;
	mlk(&idbc->cp_m);
	while(idbc->cp_exit == 0) {
		rc = get_time(&ts);
		if (rc != 0) {
			WARN("get_time gave errno=%i, cannot start "
			     "checkpoint thread\n", errno);
			break;
		}
		ts.tv_sec += idb_checkpoint_interval;
		ts.tv_nsec += 0;
		if (ts.tv_nsec >= 1000000000) {
			ts.tv_sec += 1;
			ts.tv_nsec -= 1000000000;
		}
		rc = pthread_cond_timedwait(&idbc->cp_c, &idbc->cp_m, &ts);
		if (rc != 0 && rc != ETIMEDOUT) {
			assert(0);
			abort();
		}
		if ((rc = idbc->env->txn_checkpoint(idbc->env, 0, 0, 0)) != 0) {
			WARN("Checkpoint failed, txn_checkpoint: %s\n",
			     db_strerror(rc));
		}
		if ((rc = idbc->env->log_archive(idbc->env, &list, DB_ARCH_ABS)) != 0) {
			WARN("log_archive: %s\n", db_strerror(rc));
		}
		if (list != NULL) {
			for (begin = list; *list != NULL; ++list)
				if ((rc = remove(*list)) != 0) {
					WARN("remove %s: %s\n", *list,
					     strerror(errno));
				}
			free (begin);
		}
	}
	munlk(&idbc->cp_m);
	return NULL;
}

static int
open_env(IDBC idbc, const char *envpath)
{
	int rc;
	u_int32_t envflags, maxtxns;

	rc = db_env_create(&idbc->env, 0);
	if(rc != 0) {
		ERR("db_env_create: %s\n", db_strerror(rc));
		errno = rc;
		return -1;
	}
	idbc->env->set_data_dir(idbc->env, "./");
	idbc->env->set_cachesize(idbc->env, 0, 8*1024*1024, 1);
	idbc->env->mutex_set_max(idbc->env, 32*1024);
	idbc->env->set_tx_max(idbc->env, 4096);

	rc = idbc->env->set_lk_detect(idbc->env, DB_LOCK_MINLOCKS);
	if (rc != 0) {
		WARN("Could not set deadlock detection dynamically\n");
	}
	envflags = DB_CREATE | DB_INIT_LOCK |
		DB_INIT_LOG | DB_INIT_MPOOL |
		DB_INIT_TXN | DB_RECOVER | DB_THREAD;

	rc = idbc->env->open(idbc->env, envpath, envflags, 0);
	if(rc != 0) {
		ERR("idbc->env->open: %s\n", db_strerror(rc));
		idbc->env->close(idbc->env, 0);
		errno = rc;
		return rc;
	}
	if ((idbc->flags & IDB_USE_SNAPSHOT)) {
		if (_idb_debug)
			printf("Using DB_MULTIVERSION\n");
		rc = idbc->env->set_flags(idbc->env, DB_MULTIVERSION, 1);
		if (rc != 0) {
			ERR("idbc->env->set_flags: %s\n", db_strerror(rc));
			idbc->env->close(idbc->env, 0);
			errno = rc;
			return rc;
		}
	}

	/* Nothing beyond here is an error, just suboptimal. */
	rc = idbc->env->set_flags(idbc->env, DB_LOG_AUTO_REMOVE, 1);
	if (rc != 0) {
		WARN("idbc->env->setflags(DB_LOG_AUTO_REMOVE): %s\n",
		     db_strerror(rc));
	} else {
		rc = pthread_create(&idbc->cp_thread, NULL, checkpointer,
				    idbc);
		if (rc != 0) {
			WARN("Creating checkpoint thread: %s\n",
			     strerror(rc));
		}
	}
	if (idb_verbose) {
		idbc->env->get_tx_max(idbc->env, &maxtxns);
		printf("IDB can use up to %u txns\n", maxtxns);
	}
	return 0;
}

static int
start_aebdb(void)
{
	int rc = 0;
#ifdef USE_AESOP
	rc = aebdb_init();
#endif
	return rc;
}

static void
stop_aebdb(void)
{
#ifdef USE_AESOP
	aebdb_finalize();
#endif
}

static int
table_chain_open(const char *name, DB **db, int use_int_cmp, IDBC c,  int *rc)
{
	if (*rc) {
		*db = NULL;
		return -1;
	}
	*rc = open_db(name, 0, db, c, use_int_cmp);
	if (*rc) {
		*db = NULL;
		return -1;
	}
	return 0;
}

static int
table_chain_close(DB **db)
{
	if (*db == NULL) {
		return -1;
	}
	close_db(*db);
	*db = NULL;
	return 0;
}

static int
close_tables(IDBC c)
{
	table_chain_close(&c->id_bdb);
	table_chain_close(&c->shared_bdb);
	table_chain_close(&c->admin);
	return 0;
}

static int
open_tables(IDBC c)
{
	int rc = 0;

	(void)table_chain_open("ids", &c->id_bdb, 0, c, &rc);
	(void)table_chain_open("idbs", &c->shared_bdb, 1, c, &rc);
	(void)table_chain_open("admin", &c->admin, 0, c, &rc);
	if (rc != 0) {
		(void)close_tables(c);
	}
	return rc;
}

IDBC
idb_init(const char *path, idb_flags_t flags)
{
	int rc;
	IDBC c = NULL;

	rc = start_aebdb();
	if (rc == 0) {
		c = malloc(sizeof(*c));
		if (c != NULL) {
			(void)pthread_mutex_init(&c->cp_m, NULL);
			(void)pthread_cond_init(&c->cp_c, NULL);
			c->cp_exit = 0;
			c->flags = flags;
			rc = open_env(c, path);
			if (rc == 0) {
				rc = open_tables(c);
				if (rc == 0) {
					/* Success, fall through */
				} else {
					ERR("Could not open shared table "
					    "(%s)\n", db_strerror(rc));
					rc = errno;
					close_env(c);
					stop_aebdb();
					free(c);
					c = NULL;
					errno = rc;
				}
			} else {
				ERR("Could not open BDB environment (%s).\n",
				    db_strerror(rc));
				rc = errno;
				stop_aebdb();
				free(c);
				c = NULL;
				errno = rc;
			}
		}
	} else {
		rc = errno;
		ERR("Could not start aebdb: %s\n", db_strerror(rc));
		errno = rc;
	}
	return c;
}

static void
close_env(IDBC c)
{
	int rc;

	rc = 0;
	mlk(&c->cp_m);
	c->cp_exit = 1;
	csig(&c->cp_c);
	munlk(&c->cp_m);

	rc = pthread_join(c->cp_thread, NULL);
	if (rc != 0) {
		assert(0);
		abort();
	}

	if(c->env != NULL) {
		rc = c->env->close(c->env, 0);
		if(rc != 0)
			fprintf(stderr, "error closing env\n");
	} else {
		assert(0);
		abort();
	}
}

void
idb_fini(IDBC c)
{

	close_tables(c);
	close_env(c);
	stop_aebdb();
	free(c);

	return;
}

static int
open_db(const char *dbname, int rdonly, DB **bdbp, IDBC c, int use_int_cmp)
{
	int rc;
	DB *bdb;
	u_int32_t dbflags;

	(void)rdonly;
	rc = db_create(&bdb, c->env, 0);
	if(rc != 0) {
		ERR("db_create: %s\n", db_strerror(rc));
		return rc;
	}
	*bdbp = bdb;

	dbflags = DB_CREATE | DB_AUTO_COMMIT | DB_THREAD;
	if (use_int_cmp) {
		rc = bdb->set_bt_compare(bdb, idb_comp);
		if (rc) {
			ERR("set_bt_compare: %s\n", db_strerror(rc));
			bdb->close(bdb, 0);
			return rc;
		}
	} else {
		//rc = bdb->set_bt_compare(bdb, mymemcmp);
	}
	rc = bdb->open(bdb, NULL, dbname,
			   NULL, DB_BTREE, dbflags, 0);
	if(rc != 0) {
		ERR("db->open: %s\n", db_strerror(rc));
		bdb->close(bdb, 0);
		return rc;
	}

	return 0;
}

static int
set_db_id_of_key(DB *db, DB_TXN *tx, const char *path, size_t plen,
		 idb_db_id id)
{
	DBT key, data;

	memset(&key, '\0', sizeof(key));
	memset(&data, '\0', sizeof(data));
	key.data = path;
	key.size = plen;
	data.data = &id;
	data.size = sizeof(id);

	return db->put(db, tx, &key, &data, 0);
}

static int
get_db_id_of_key(DB *db, DB_TXN *tx, const char *path, size_t plen,
		 idb_db_id *id)
{
	DBT key, data;

	memset(&key, '\0', sizeof(key));
	memset(&data, '\0', sizeof(data));
	key.data = path;
	key.ulen = plen;
	key.size = key.ulen;
	key.flags = DB_DBT_USERMEM;
	data.data = id;
	data.ulen = sizeof(*id);
	data.size = data.ulen;
	data.flags = DB_DBT_USERMEM;

	return db->get(db, tx, &key, &data, DB_RMW);
}

static int
get_db_id(IDBC c, const char *path, idb_db_id *id)
{
	int rc = 0;
	DB_TXN *tx = NULL;
	static char maxdbid[] = "db_max_db_id";

	if ((c->flags & IDB_SHARED_TABLE) == 0) {
		*id = 0;
		return 0;
	}

	do {
		rc = c->env->txn_begin(c->env, NULL, &tx, 0);
		if (rc) break;

		rc = get_db_id_of_key(c->id_bdb, tx, path, strlen(path)+1,
				      id);
		if (rc == DB_NOTFOUND) {
			rc = get_db_id_of_key(c->admin, tx, maxdbid,
					      sizeof(maxdbid), id);
			if (rc == DB_NOTFOUND)
				*id = 0;
			else if (rc != 0)
				break;
			(*id)++;
			rc = set_db_id_of_key(c->id_bdb, tx, path,
					      strlen(path)+1, *id);
			if (rc) break;
			rc = set_db_id_of_key(c->admin, tx, maxdbid,
					      sizeof(maxdbid), *id);
			if (rc) break;
		}
	} while(0);

	if (rc == 0) {
		rc = tx->commit(tx, 0);
	} else {
		tx->abort(tx);
	}
	return rc;
}

IDB
idb_open(const char *path, int rdonly, IDBC c)
{
	IDB ret;
	int rc;
	char *newpath;

	ret = malloc(sizeof(*ret));
	newpath = malloc(strlen(path) + 4 + 1);
	if (ret != NULL && newpath != NULL) {
		ret->c = c;
		ret->rdonly = rdonly;
		strcpy(newpath, path);
		strcat(newpath, ".idb");
		rc = get_db_id(c, path, &ret->db_id);
		if (rc == 0) {
			if (c->flags & IDB_SHARED_TABLE) {
				ret->bdb = c->shared_bdb;
			} else {
				rc = open_db(newpath, rdonly, &ret->bdb, c, 1);
				if (rc != 0) {
					ERR("open_db: %s", db_strerror(rc));
					free(ret);
					ret = NULL;
				} else {
					/* Success, fall through */
				}
			}
		} else {
			if (rc != DB_LOCK_DEADLOCK)
				ERR("get_db_id: %s", db_strerror(rc));
			free(ret);
			ret = NULL;
		}
	} else {
		rc = ENOMEM;
		free(ret);
	}
	free(newpath);
	RCMAP(&rc);
	errno = rc;
	return ret;
}

static void
close_db(DB *bdb)
{
	int rc = 0;

	if(bdb != NULL)	{
		rc = bdb->close(bdb, 0);
		if(rc != 0)
			WARN("db->close: %s\n", db_strerror(rc));
	}
}

int
idb_close(IDB idb)
{

	if ((idb->c->flags & IDB_SHARED_TABLE) == 0)
		close_db(idb->bdb);
	free(idb);
	return 0;
}

const char *
idb_strerror(int errnum)
{
	RCUNMAP(&errnum);
	return db_strerror(errnum);
}
