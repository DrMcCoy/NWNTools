#! /bin/sh

set -x
aclocal && \
autoheader && \
automake --no-force --add-missing --copy --gnits && \
autoconf && \
./configure "$@" && \
make

