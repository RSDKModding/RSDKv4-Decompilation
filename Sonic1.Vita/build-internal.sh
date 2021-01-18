#!/bin/sh
apk add --no-cache make cmake
cmake . && \
make
rm Sonic1 # this is not ignored by .gitignore
