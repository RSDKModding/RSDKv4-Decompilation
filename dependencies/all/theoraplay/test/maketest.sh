#!/bin/sh

set -x
set -e

OSTYPE=`uname -s`
if [ "$OSTYPE" = "Linux" ]; then
    LINKFLAGS="-lpthread"
    LINKGLFLAGS="-lGL"
fi
if [ "$OSTYPE" = "Darwin" ]; then
    LINKGLFLAGS="-framework OpenGL"
fi

CFLAGS="-O0 -ggdb3 -Wall -I.."
gcc -o ./testtheoraplay $CFLAGS ../theoraplay.c ./testtheoraplay.c -logg -lvorbis -ltheoradec $LINKFLAGS
gcc -o ./simplesdl $CFLAGS ../theoraplay.c ./simplesdl.c `sdl-config --cflags --libs`  -logg -lvorbis -ltheoradec $LINKFLAGS
gcc -o ./sdltheoraplay $CFLAGS ../theoraplay.c ./sdltheoraplay.c `sdl-config --cflags --libs`  -logg -lvorbis -ltheoradec $LINKFLAGS $LINKGLFLAGS

