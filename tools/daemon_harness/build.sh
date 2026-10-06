#!/usr/bin/env bash
# Builds the daemon and the harness, then runs the harness under wine. Exits
# non-zero when a check fails, so it can gate a change.
#
# The daemon comes from daemon/build.sh -- the shipping artifact, not a copy
# compiled with different flags -- and is placed in two directories under one
# basename, which the loader treats as two separate images. That is the
# election this cannot test any other way.
set -euo pipefail
cd "$(dirname "$0")"

FLAGS=(-std=c++17 -O2 -Wall -Wextra)

../../daemon/build.sh > /dev/null
mkdir -p build/a build/b
cp ../../daemon/hideui_daemon.dll build/a/hideui_daemon.dll
cp ../../daemon/hideui_daemon.dll build/b/hideui_daemon.dll
echo "daemon : $(md5sum ../../daemon/hideui_daemon.dll | cut -d' ' -f1) into build/a and build/b"

i686-w64-mingw32-g++ "${FLAGS[@]}" \
    -static -static-libgcc -static-libstdc++ \
    -o build/daemon_harness.exe daemon_harness.cpp

echo "built  : $(pwd)/build/daemon_harness.exe"
env -u DISPLAY WINEDEBUG="${WINEDEBUG:--all}" wine build/daemon_harness.exe "$@"
