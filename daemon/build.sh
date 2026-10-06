#!/usr/bin/env bash
# Builds hideui_daemon.dll, the immortal detour service.
#
# The daemon pins itself into the client and can never be unloaded or replaced
# without the player restarting the game, so this script gates the two things
# that cannot be fixed afterwards:
#
#   - exactly one export, undecorated, named hu_daemon_acquire (the engine
#     resolves it by name; a decorated name would be a silent failure);
#   - no static constructors and no atexit registrations in our object, because
#     anything registered there runs at process exit whatever DllMain does.
set -euo pipefail
cd "$(dirname "$0")"

FLAGS=(-std=c++17 -O2 -Wall -Wextra -Wpedantic)

i686-w64-mingw32-g++ "${FLAGS[@]}" -c -o hideui_daemon.o hideui_daemon.cpp

# No dynamic initialisers, no exit-time work. GCC names a translation unit's
# initialiser _GLOBAL__sub_I_*; a non-trivial destructor on a namespace-scope
# object shows up as a reference to atexit or __cxa_atexit.
symbols=$(i686-w64-mingw32-nm hideui_daemon.o)
if grep -q '_GLOBAL__sub_I' <<< "$symbols"; then
    echo "BUILD FAILED: the daemon has a static initialiser"
    grep '_GLOBAL__sub_I' <<< "$symbols"
    exit 1
fi
if grep -qE '(^| )U .*(atexit|__cxa_atexit)' <<< "$symbols"; then
    echo "BUILD FAILED: the daemon registers something to run at process exit"
    grep -E '(^| )U .*(atexit|__cxa_atexit)' <<< "$symbols"
    exit 1
fi

# --no-insert-timestamp makes the link reproducible, so "the deployed daemon is
# the daemon we tested" is an md5 comparison.
i686-w64-mingw32-g++ "${FLAGS[@]}" \
    -shared -static -static-libgcc -static-libstdc++ -Wl,--no-insert-timestamp \
    -o hideui_daemon.dll hideui_daemon.o exports.def
rm -f hideui_daemon.o

# grep -q closes the pipe early, which trips pipefail via SIGPIPE -- read the
# exports into a variable first.
dump=$(i686-w64-mingw32-objdump -p hideui_daemon.dll)
exports=$(sed -n '/\[Ordinal\/Name Pointer\] Table/,/^$/p' <<< "$dump" | grep -oE '\[ *[0-9]+\] .*' | sed 's/.*\] //')
if ! grep -qx 'hu_daemon_acquire' <<< "$exports"; then
    echo "BUILD FAILED: hu_daemon_acquire is not exported"
    echo "$exports"
    exit 1
fi
count=$(grep -c . <<< "$exports")
if [ "$count" != "1" ]; then
    echo "BUILD FAILED: expected exactly one export, found $count"
    echo "$exports"
    exit 1
fi

echo "built  : $(pwd)/hideui_daemon.dll"
echo "export : hu_daemon_acquire (the only one)"
echo "statics: no dynamic initialisers, no exit-time registrations"
echo "md5    : $(md5sum hideui_daemon.dll | cut -d' ' -f1)"
echo "NOT deployed. The daemon ships in each addon's own libs/ folder."
