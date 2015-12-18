#!/bin/bash

# this example:
# - sets alignment to page size (4096)
# - syncs each write before completion
# - turns on sync coalescing (highwater of 16 means that up to 16 operations
#   will be synced at once)
# - writes to a /tmp directory
#
tests/recordstore-basic file "alignment:4096,O_SYNC,sync_highwater:16,path:/tmp/recordstore-$$"
if [ $? -ne 0 ]; then
    exit 1
fi

ls /tmp/recordstore-$$
rm -rf /tmp/recordstore-$$

exit 0
