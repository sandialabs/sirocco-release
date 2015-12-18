#include <assert.h>
#include <idb.h>
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char **argv)
{
	IDBC c;
	IDBX x;
	char *next, *prev;

	if ((c = idb_init("0", IDB_SHARED_TABLE)) == NULL) {
		if (errno == ENOENT) {
			perror("idb_init (be sure that an IDB exists first)");
		} else {
			perror("idb_init");
		}
		assert(0);
		abort();
	}
	if ((x = idb_begin(c)) == NULL) {
		perror("idb_begin");
		assert(0);
		abort();
	}
	next = malloc(1);
	if (next == NULL) {
		perror("malloc");
		assert(0);
		abort();
	}
	next[0] = '\0';

	next = idb_next(x, next, 0);
	while (next != NULL) {
		if (next != NULL)
			printf("Found db %s\n", next);
		prev = next;
		next = idb_next(x, next, IDB_NEXT_EXCL);
		free(prev);
	}
	(void)idb_end(x, 0);
	idb_fini(c);
	return 0;
}
