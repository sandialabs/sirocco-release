#ifndef _IDB_H
#define _IDB_H

#include <errno.h>
#include <stdint.h>
#include <time.h>

typedef uint64_t idb_vers_t;
typedef int64_t idb_off_t;
typedef uint64_t idb_size_t;
typedef uint64_t idb_flags_t;

#ifndef UINT64_MIN
#define UINT64_MIN 0
#endif

/* Errors */
#define IDB_DEADLOCK EDEADLOCK
#define IDB_RESTART  ENOTRECOVERABLE

/* Limits */
#define IDB_VERS_MIN    UINT64_MIN
#define IDB_VERS_MAX	UINT64_MAX
#define IDB_OFF_MAX	INT64_MAX
#define IDB_OFF_MIN     INT64_MIN
#define IDB_SIZE_MIN    UINT64_MIN
#define IDB_SIZE_MAX	UINT64_MAX

/* Size of user-defined data in record */
#define IDB_UDSZ 32

/* Flag that determines whether idb_next chooses next record on match */
#define IDB_NEXT_EXCL 1

/* interval of contiguous physical storage with logical mapping & metadata */
struct idb_interval {
	idb_vers_t vers;       /* version number                           */
	idb_off_t rec;         /* record id                                */
	idb_size_t reclen;     /* record length, bytes                     */
	idb_size_t size;       /* size of contiguous records, in records   */
	idb_size_t log_offset; /* offset, automatically updated on splits  */
	idb_size_t epoch;      /* IDB epoch number for validated data      */
	idb_flags_t flags;     /* IDB flags: punch, valid, syncing, syncd  */
	char data[IDB_UDSZ];   /* Generic user data */
};

typedef struct _idb *IDB;
typedef struct _idb_transaction *IDBX;
typedef struct _idb_context *IDBC;

extern struct _idb_stats {
	unsigned long long ngets;
	unsigned long long nhits;
} _idb_statistics;

extern int _idb_debug;
extern void (*_idb_diagnostic)(const char *s);

extern IDBC idb_init(const char *path, idb_flags_t flags);

#define IDB_USE_SNAPSHOT (1 << 0)
#define IDB_SHARED_TABLE (1 << 1)

extern void idb_fini(IDBC c);
extern IDB idb_open(const char *path, int rdonly, IDBC c);
extern int idb_close(IDB idb);
extern IDBX idb_begin(IDBC c);
extern int idb_precommit(IDBX idbx);
extern int idb_end(IDBX idbx, int commit);
extern int idb_insert(IDBX idbx, IDB idb, struct idb_interval *iv);
extern int idb_remove(IDBX idbx, IDB idb, struct idb_interval *iv);
extern int idb_stab(IDBX idbx,
		    IDB idb,
		    struct idb_interval *iv,
		    int (*f)(const struct idb_interval *, void *),
		    void *arg);
extern const char *idb_strerror(int errnum);
extern time_t idb_checkpoint_interval;
extern int idb_verbose;

extern char *idb_next(IDBX idbx, char *path, unsigned flags);
#endif /* !_IDB_H */
