#include <stdint.h>
#include <time.h>
#include "idb.h"
#include <db.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include "idb_internal.h"

time_t idb_checkpoint_interval = 60;
int idb_verbose = 0;

int _idb_debug;

struct _idb_stats _idb_statistics;
