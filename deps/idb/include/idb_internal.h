#ifndef __IDB_INTERNAL_H__
#define __IDB_INTERNAL_H__

#include <db.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>

#include "idb.h"

typedef uint64_t idb_db_id;

struct _idb {
	DB *bdb;
	int rdonly;
	idb_db_id db_id;
	IDBC c;
};

struct _idb_transaction {
	DB_TXN *tx;
	unsigned success:1;
	IDBC c;
};

struct _idb_context {
	idb_flags_t flags;
	pthread_mutex_t cp_m;
	pthread_cond_t cp_c;
	int cp_exit;
	pthread_t cp_thread;
	DB_ENV *env;
	DB *id_bdb;
	DB *shared_bdb;
	DB *admin;
};

#ifdef USE_VALGRIND
#include <valgrind/valgrind.h>
#include <valgrind/memcheck.h>
#define VGCHECK(x) VALGRIND_CHECK_MEM_IS_DEFINED(x, sizeof(*x))
#else
#define VGCHECK(x)
#endif

#define VGCHECK_AND_SET(x,y) do {		\
		*x = *y;			\
		VGCHECK(x);			\
	} while(0)

#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)

#define PRINT_STATUS(force, prepend, ...) if (force || idb_verbose)	\
		fprintf(stderr, prepend __FILE__ ","			\
			STRINGIFY(__LINE__)				\
			": " __VA_ARGS__)

#define WARN(...) do {							\
		PRINT_STATUS(1, "IDB-BDB WARNING @", __VA_ARGS__);	\
	} while(0)							\

#define ERR(...) do {							\
		PRINT_STATUS(1, "IDB-BDB ERROR @", __VA_ARGS__);	\
	} while(0)


/* For DB-specific errors, remap to something that users know about */
#define RCMAP(p)				\
	do {					\
		switch(*(p)) {			\
		case DB_LOCK_DEADLOCK:		\
			*(p) = IDB_DEADLOCK;	\
			break;			\
		case DB_RUNRECOVERY:		\
			(*p) = IDB_RESTART;	\
			break;			\
		default:			\
			break;			\
		}				\
	} while(0)

#define RCUNMAP(p)					\
	do {						\
		switch(*(p)) {				\
		case IDB_DEADLOCK:			\
			*(p) = DB_LOCK_DEADLOCK;	\
			break;				\
		case IDB_RESTART:			\
			(*p) = DB_RUNRECOVERY;		\
			break;				\
		default:				\
			break;				\
		}					\
	} while(0)

#endif
