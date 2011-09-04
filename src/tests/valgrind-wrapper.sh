#!/bin/sh

# RUN IN VALGRIND, WITH LEAK CHECKING ENABLED
# run it via the libtool in order to properly
# report issues in the shared libraries
libtool --mode=execute \
    valgrind -q --tool=memcheck --leak-check=full --error-exitcode=1 "$@"

exit $?
