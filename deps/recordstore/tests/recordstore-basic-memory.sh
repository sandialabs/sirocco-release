#!/bin/bash

tests/recordstore-basic memory ""
if [ $? -ne 0 ]; then
    exit 1
fi

exit 0
