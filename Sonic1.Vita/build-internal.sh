#!/bin/sh
apk add --no-cache make cmake
cmake . && \
make -j$(nproc)
rm Sonic1 # this is not ignored by .gitignore
