#!/bin/bash

# export a few mandatory environment variables
export LIBDELOREAN_CPPPATH="$(pwd)/contrib/libdelorean/include"
export LIBDELOREAN_LIBPATH="$(pwd)/contrib/libdelorean/src"
export BABELTRACE_CPPPATH="$(pwd)/contrib/babeltrace/include"
export BABELTRACE_LIBPATH="$(pwd)/contrib/babeltrace/lib/.libs"
export BABELTRACE_CTF_LIBPATH="$(pwd)/contrib/babeltrace/formats/ctf/.libs/"
export LEVELDB_INCLUDEPATH="$(pwd)/contrib/leveldb/include"
export LEVELDB_LIBPATH="$(pwd)/contrib/leveldb"
export LD_LIBRARY_PATH="$LIBDELOREAN_LIBPATH:$BABELTRACE_LIBPATH:$BABELTRACE_CTF_LIBPATH:$LEVELDB_LIBPATH:$LD_LIBRARY_PATH"
