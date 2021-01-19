#!/bin/sh
sudo docker run \
    --rm \
    -v $PWD/..:/work \
    -w /work/Sonic2.Vita \
    vitasdk/vitasdk \
    /bin/sh -C "./build-internal.sh"
