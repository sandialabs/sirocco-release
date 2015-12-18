#include <sys/types.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#include <db.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "idb.h"
#include "idb_internal.h"
#include "idb_impl_util.h"
#include "idb_dbg.h"

/* Aesop switching functionality. This file is compiled as-is, and
 * also compiled via aeidb_impl, which just defines USE_AESOP
 * symbol. The following block defines a naming macro, as well as
 * switches BDB calls to aesop or non-aesop variants.
 *
 * Note that some bdb functions, at the time of writing, did not have
 * aesop equivalents.
 */

#ifdef USE_AESOP
#include "aeidb.hae"
#define IDB_NM(x) aeidb_ ## x
#define MAYBLOCK __blocking
#define bdb_txn_begin aebdb_txn_begin
#define bdb_txn_commit aebdb_txn_commit
#define bdb_txn_abort aebdb_txn_abort
#define bdb_cursor aebdb_cursor
#define bdb_cursor_get aebdb_cursor_get
#define bdb_cursor_close aebdb_cursor_close
#define bdb_put aebdb_put
#define bdb_tx_prepare(tx, ...) tx->prepare(tx, __VA_ARGS__)
#define bdb_cursor_c_get(cur, ...) cur->c_get(cur, __VA_ARGS__)
#define bdb_cursor_c_put(cur, ...) cur->c_put(cur, __VA_ARGS__)
#define bdb_cursor_del(cur, ...) cur->del(cur, __VA_ARGS__)
#include <aesop/aebdb.hae>
#else
#define IDB_NM(x) idb_ ## x
#define MAYBLOCK
#define bdb_txn_begin(env, ...) env->txn_begin(env, __VA_ARGS__)
#define bdb_txn_commit(tx, ...) tx->commit(tx, __VA_ARGS__)
#define bdb_txn_abort(tx) tx->abort(tx)
#define bdb_cursor(dbp, ...) dbp->cursor(dbp, __VA_ARGS__)
#define bdb_cursor_get(cur, ...) cur->get(cur, __VA_ARGS__)
#define bdb_cursor_close(cur) cur->close(cur)
#define bdb_tx_prepare(tx, ...) tx->prepare(tx, __VA_ARGS__)
#define bdb_cursor_c_get(cur, ...) cur->c_get(cur, __VA_ARGS__)
#define bdb_cursor_c_put(cur, ...) cur->c_put(cur, __VA_ARGS__)
#define bdb_cursor_del(cur, ...) cur->del(cur, __VA_ARGS__)
#define bdb_put(db, ...) db->put(db, __VA_ARGS__)
#endif

static void
dbg_ddlk(IDBX x)
{
	if (_idb_dbg_ddlk) {
		if ((rand() % _idb_dbg_ddlk) == 0)
			x->success = 0;
	}
}

/* Implementation for idb_begin and aeidb_begin */
MAYBLOCK IDBX
IDB_NM(begin)(IDBC c)
{
	IDBX tx;
	int rc;

	tx = malloc(sizeof(*tx));
	if (tx != NULL) {
		tx->c = c;
		memset(&tx->tx, 0, sizeof(tx->tx));
		tx->success = 1;
		rc = bdb_txn_begin(c->env, NULL, &tx->tx, DB_TXN_SNAPSHOT);
		if (rc != 0) {
			ERR("txn_begin: %s\n", db_strerror(rc));
			free(tx);
			tx = NULL;
			if (rc == DB_RUNRECOVERY)
				rc = ENOTRECOVERABLE;
		}
	} else {
		rc = ENOMEM;
	}
	errno = rc;
	return tx;
}

/* Implementation for idb_end and aeidb_end */
MAYBLOCK int
IDB_NM(end)(IDBX idbx, int commit)
{
	int rc;

	if (commit && idbx->success == 0) {
		rc = IDB_NM(end)(idbx, 0);
		if (rc == 0)
			rc = IDB_DEADLOCK;
		RCMAP(&rc);
		errno = rc;
		return rc;
	}

	if (commit) {
		rc = bdb_txn_commit(idbx->tx, 0);
	} else {
		rc = bdb_txn_abort(idbx->tx);
	}
	if (rc != 0 && rc != DB_LOCK_DEADLOCK)
		ERR("tx->%s: %s\n", (commit) ? "commit" : "abort",
		    db_strerror(rc));
	free(idbx);
	RCMAP(&rc);
	errno = rc;
	return rc;
}

static void
copy_iv(struct idb_interval *dst, struct idb_interval *src)
{
	memset(dst, 0, sizeof(*dst));
	VGCHECK_AND_SET(&dst->vers,       &src->vers);
	VGCHECK_AND_SET(&dst->rec,        &src->rec);
	VGCHECK_AND_SET(&dst->reclen,     &src->reclen);
	VGCHECK_AND_SET(&dst->size,       &src->size);
	VGCHECK_AND_SET(&dst->log_offset, &src->log_offset);
	VGCHECK_AND_SET(&dst->epoch,      &src->epoch);
	VGCHECK_AND_SET(&dst->flags,      &src->flags);
	memcpy(dst->data, src->data, IDB_UDSZ);
#ifdef USE_VALGRIND
	VALGRIND_CHECK_MEM_IS_DEFINED(src->data, IDB_UDSZ);
#endif
	VGCHECK(dst);
	return;
}

struct db_key_data {
	uint64_t d[2];
};

static void
db_key_set(DBT *key, idb_off_t rec, IDB idb)
{
	struct db_key_data *dkd;

	dkd = key->data;
	if (idb->c->flags & IDB_SHARED_TABLE) {
		key->size = 2 * sizeof(dkd->d[0]);
		dkd->d[0] = idb->db_id;
		dkd->d[1] = rec;
	} else {
		key->size = sizeof(dkd->d[0]);
		dkd->d[0] = rec;
	}
}

static int
db_key_valid(DBT *key, IDB idb)
{
	struct db_key_data *dkd = key->data;

	if (idb->c->flags & IDB_SHARED_TABLE) {
		return (dkd->d[0] == idb->db_id);
	} else {
		return 1;
	}
}

MAYBLOCK static int
do_insert(struct idb_interval *iv, DB_TXN *tx, IDB idb, DBC *cur)
{
	DBT key, data;
	struct db_key_data dkd;
	int rc;

	struct idb_interval ins;
	memset(&ins, 0, sizeof(ins));

	copy_iv(&ins, iv);

	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));

	key.data = dkd.d;
	db_key_set(&key, iv->rec + (idb_off_t)iv->size, idb);
	data.data = &ins;
	data.size = sizeof(ins);
	if (cur == NULL) {
		VGCHECK(&key);
		VGCHECK(&data);
		rc = bdb_put(idb->bdb, tx, &key, &data, 0);
		if (rc != 0 && rc != DB_LOCK_DEADLOCK)
			ERR("db->put: %s\n", db_strerror(rc));
	} else {
		rc = bdb_cursor_c_put(cur, &key, &data, 0);
		if (rc != 0 && rc != DB_LOCK_DEADLOCK)
			ERR("cur->cput: %s\n", db_strerror(rc));
	}

	return rc;
}

/* Split intervals in database around iv.
 * Removes conflicting intervals in database.
 * Returns surviving intervals in ivs. Must be able to hold two intervals.
 *
 * If this returns an error, the transaction may be damaged and should
 * be aborted.
 *
 * Paramters:
 * - cur: A cursor for the database. Provided to allow iteration.
 * - iv: The interval the be split around.
 * - do_ins: Whether iv should be inserted into the database.
 * - idbx: The transaction to use for addition/removal.
 * - idb: The database to operate upon.
 *
 * TODO: Refactor
 */
MAYBLOCK static int
do_split(DBC *cur, struct idb_interval *iv, int do_ins, IDBX idbx, IDB idb)
{
	DBT key, data;
	struct idb_interval *d;
	int do_rm, rc;
	size_t nivs, i;
	struct idb_interval ivs[2];
	struct db_key_data dkd;

	nivs = 0;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.data = dkd.d;
	db_key_set(&key, iv->rec, idb);

	rc = bdb_cursor_c_get(cur, &key, &data, DB_SET_RANGE);
	if (rc != 0 && rc != DB_NOTFOUND)
		return rc;

	d = data.data;
	while (rc == 0 && db_key_valid(&key, idb) &&
	       d->rec <= iv->rec + (idb_off_t)iv->size) {
		do_rm = 0;
		if (does_abut(d, iv)) {
			/* Merge logic goes here */
		}
		if (is_covered(d, iv)) {
			do_rm = 1;
		} else {
			if (does_front_survive(d, iv)) {
				do_rm = 1;
				ivs[nivs++] = *d;
				preserve_front(ivs + nivs - 1, iv);
				assert(nivs == 1);
			}
			if (does_end_survive(d, iv)) {
				do_rm = 1;
				ivs[nivs++] = *d;
				preserve_end(ivs + nivs - 1, iv);
				assert(nivs <= 2);
			}
		}

		if (do_rm) {
			rc = bdb_cursor_del(cur, 0);
			if (rc)
				break;
		}
		memset(&data, 0, sizeof(data));
		memset(&key, 0, sizeof(key));
		rc = cur->get(cur, &key, &data, DB_NEXT);
		if (rc)
			break;
		d = data.data;
	}
	if (rc == DB_NOTFOUND)
		rc = 0;

	for (i = 0; rc == 0 && i < nivs; i++) {
		rc = do_insert(ivs + i, idbx->tx, idb, NULL);
		if (rc && rc != DB_LOCK_DEADLOCK)
			fprintf(stderr, "do_insert: %s\n", db_strerror(rc));
	}
	if (rc == 0 && do_ins)
		rc = do_insert(iv, idbx->tx, idb, NULL);
	return rc;
}

/* If do_del is set, the interval described should not be added to the
 * database.
 */
MAYBLOCK static int
__idb_addrem(IDBX idbx, IDB idb, struct idb_interval *iv, int do_del)
{
	int rc, err;
	DB *dbp;
	DBC *cur;

	if (is_valid_interval(iv) == 0) {
		ERR("Interval outside of domain.");
		return EDOM;
	}

	if (idb->rdonly) {
		ERR("Tried to modify a read-only database.\n");
		return EPERM;
	}

	if (iv->size == 0)
		return 0;

	dbp = idb->bdb;
	rc = bdb_cursor(dbp, idbx->tx, &cur, DB_TXN_SNAPSHOT);
	if (rc != 0) {
		if (rc != DB_LOCK_DEADLOCK)
			fprintf(stderr, "bdb_cursor: %s\n",
				db_strerror(rc));
		return rc;
	}

	rc = do_split(cur, iv, !do_del, idbx, idb);

	err = rc;
	rc = bdb_cursor_close(cur);
	if (err == 0 && rc)
		err = rc;

	return err;
}

/* Implementation for idb_stab and aeidb_stab */
static MAYBLOCK int
IDB_NM(int_stab)(IDBX idbx,
		 IDB idb,
		 struct idb_interval *iv,
		 MAYBLOCK int (*f)(const struct idb_interval *, void *),
		 void *arg)
{
	int rc, more, err;
	DBT key, data;
	DBC *cur;
	struct idb_interval *civ;
	struct db_key_data dkd;

	if (is_valid_interval(iv) == 0) {
		ERR("Interval outside of domain.");
		return EDOM;
	}

	rc = bdb_cursor(idb->bdb, idbx->tx, &cur, DB_TXN_SNAPSHOT);
	if (rc != 0)
		return rc;

	memset(&key, 0, sizeof(key));
	key.data = dkd.d;
	db_key_set(&key, iv->rec, idb);
	memset(&data, 0, sizeof(data));

	rc = bdb_cursor_get(cur, &key, &data, DB_SET_RANGE);

	civ = data.data;
	/* This is tricky. For an empty database, I would have assumed
	 * that rc would have been DB_NOTFOUND. However, in practice,
	 * I'm seeing data.data == NULL, and the key is
	 * unmodified. I'll just check for that, too.
	 */
	while (rc == 0 && data.data != NULL &&
	       db_key_valid(&key, idb) &&
	       civ->rec < iv->rec + (idb_off_t)iv->size) {
		/* Querying on the rec of incoming interval can return the
		   directly preceding interval. Skip it. */
		if (does_abut(civ, iv) == 0) {
			more = f(civ, arg);
			if (more == 0) {
				break;
			}
		}
		memset(&data, 0, sizeof(data));
		memset(&key, 0, sizeof(key));
		rc = bdb_cursor_get(cur, &key, &data, DB_NEXT);
		if (rc == DB_NOTFOUND)
			break;
		civ = data.data;
	}

	if (rc == DB_NOTFOUND)
		rc = 0;
	err = bdb_cursor_close(cur);
	if (rc || err)
		idbx->success = 0;
	if (rc == 0 && err)
		rc = err;
	return rc;
}

/* Implementation for idb_precommit and aeidb_precommit */
MAYBLOCK int
IDB_NM(precommit)(IDBX idbx)
{
	u_int8_t bigtx[128];
	int rc;

	rc = errno = IDB_DEADLOCK;
	if (idbx->success) {
		memset(bigtx, 0, sizeof(bigtx));
		rc = bdb_tx_prepare(idbx->tx, bigtx);
		RCMAP(&rc);
		errno = rc;
		if (rc)
			idbx->success = 0;
	}
	return rc;
}

MAYBLOCK int
IDB_NM(stab)(IDBX idbx,
	     IDB idb,
	     struct idb_interval *iv,
	     MAYBLOCK int (*f)(const struct idb_interval *, void *),
	     void *arg)
{
	int rc;

	rc = errno = IDB_DEADLOCK;
	dbg_ddlk(idbx);
	if (idbx->success) {
		rc = IDB_NM(int_stab(idbx, idb, iv, f, arg));
		RCMAP(&rc);
		errno = rc;
		if (rc)
			idbx->success = 0;
	}
	return rc;
}

/* Implementation for idb_remove and aeidb_remove */
MAYBLOCK int
IDB_NM(remove)(IDBX idbx, IDB idb, struct idb_interval *iv)
{
	int rc;

	rc = errno = IDB_DEADLOCK;
	dbg_ddlk(idbx);
	if (idbx->success) {
		rc =  __idb_addrem(idbx, idb, iv, 1);
		RCMAP(&rc);
		errno = rc;
		if (rc)
			idbx->success = 0;
	}
	return rc;
}

/* Implementation for idb_insert and aeidb_insert */
MAYBLOCK int
IDB_NM(insert)(IDBX idbx, IDB idb, struct idb_interval *iv)
{
	int rc;

	rc = errno = IDB_DEADLOCK;
	dbg_ddlk(idbx);
	if (idbx->success) {
		rc =  __idb_addrem(idbx, idb, iv, 0);
		RCMAP(&rc);
		errno = rc;
		if (rc)
			idbx->success = 0;
	}
	return rc;
}

/* Given a db name, gets the next one available (inclusive). Currently
   only works with SHARED_TABLE. */
static MAYBLOCK char *
do_next(IDBX idbx, char *path, unsigned flags)
{
	int rc;
	DBC *cur;
	DBT key, data;
	char *ret = NULL;

	rc = idbx->c->id_bdb->cursor(idbx->c->id_bdb,
				     idbx->tx,
				     &cur, 0);
	if (rc)
		abort();
	memset(&key, 0, sizeof(key));
	key.data = path;
	key.size = strlen(key.data)+1;
	memset(&data, 0, sizeof(data));
	rc = bdb_cursor_get(cur, &key, &data, DB_SET_RANGE);

	if (rc == 0 && strcmp(key.data, path) == 0 &&
	    (flags & IDB_NEXT_EXCL)) {
		rc = bdb_cursor_get(cur, &key, &data, DB_NEXT);
	}

	if (rc == 0) {
		ret = malloc(key.size);
		if (ret != NULL) {
			memcpy(ret, key.data, key.size);
		} else {
			errno = ENOMEM;
		}
	}
	if (rc)
		errno = rc;
	cur->c_close(cur);
	return ret;
}

MAYBLOCK char *
IDB_NM(next)(IDBX idbx, char *path, unsigned flags)
{
	char *ret = NULL;
	int rc;
	(void)rc;

	rc = errno = IDB_DEADLOCK;
	if (idbx->success) {
		if (idbx->success) {
			ret = do_next(idbx, path, flags);
		}
	}
	return ret;
}
