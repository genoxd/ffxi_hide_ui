#!/bin/bash
# Cross-compiles _HideUI.dll (32-bit PE) into examples/hideuidemo/libs and ships
# hideui_daemon.dll beside it. Same toolchain and LuaCore import library as
# the proof of concept.
#
#   build.sh [output]   output: a file name in examples/hideuidemo/libs
#                       (default _HideUI.dll), or a path to a folder that
#                       exists. HIDEUI_DEFINES adds compiler defines; the
#                       tests build one copy with them.
set -euo pipefail

ENGINE_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(dirname "$ENGINE_DIR")"
FFXI="$(dirname "$ROOT")"
ADDON_DIR="$ROOT/examples/hideuidemo"
DAEMON="$ROOT/daemon/hideui_daemon.dll"
LUA_INC="${LUA_INC:-/usr/include/lua5.1}"
OUT_ARG="${1:-_HideUI.dll}"
case "$OUT_ARG" in
    */*)
        [ -d "$(dirname "$OUT_ARG")" ] || { echo "Error: $(dirname "$OUT_ARG") does not exist" >&2; exit 1; }
        OUT_DIR="$(cd "$(dirname "$OUT_ARG")" && pwd)"
        ;;
    *)
        OUT_DIR="$ADDON_DIR/libs"
        ;;
esac
OUT="$OUT_DIR/$(basename "$OUT_ARG")"
OBJ="$ENGINE_DIR/_HideUI.o"

command -v i686-w64-mingw32-g++ >/dev/null 2>&1 || {
    echo "Error: MinGW-w64 not found (sudo apt-get install mingw-w64)" >&2
    exit 1
}
[ -f "$LUA_INC/lua.h" ] || { echo "Error: lua.h not under $LUA_INC" >&2; exit 1; }
[ -f "$ROOT/libLuaCore.a" ] || { echo "Error: libLuaCore.a missing" >&2; exit 1; }
[ -f "$DAEMON" ] || { echo "Error: $DAEMON missing; run daemon/build.sh" >&2; exit 1; }

# The addon tree must already exist: the live client cannot create folders, and
# a write into a missing one freezes it.
for d in "$ADDON_DIR" "$ADDON_DIR/libs" "$ADDON_DIR/data"; do
    [ -d "$d" ] || { echo "Error: $d does not exist; create it before loading" >&2; exit 1; }
done

# Refuse to write over an image a client still has mapped: overwriting a loaded
# DLL is the deploy mistake that takes the client down. Unload the addon
# (//lua unload hideuidemo), wait a few seconds, then build. The match is on the
# output's folder and name; the proof of concept ships a DLL of the same
# basename.
mapped_by() {
    local fragment="$1" maps pid
    for maps in /proc/[0-9]*/maps; do
        if grep -qiF "$fragment" "$maps" 2>/dev/null; then
            pid=${maps#/proc/}; pid=${pid%/maps}
            echo "$pid"
            return 0
        fi
    done
    return 1
}
where="$(basename "$(dirname "$OUT_DIR")")/$(basename "$OUT_DIR")"
if pid=$(mapped_by "$where/$(basename "$OUT")"); then
    echo "Error: $where/$(basename "$OUT") is still mapped in pid $pid." >&2
    echo "       Unload the addon first: //lua unload hideuidemo" >&2
    exit 1
fi

# signatures.h holds the 30 signatures the engine scans for.
python3 - "$ENGINE_DIR/signatures.h" <<'EOF'
import re, sys
src = open(sys.argv[1], encoding='utf-8').read()
sigs = re.findall(r'const char (kSig\w+)\[\] =\s*"([^"]+)";', src)
if len(sigs) != 30:
    sys.exit('  FAIL: expected 30 signatures in signatures.h, found %d' % len(sigs))
print('  OK signatures: %d in signatures.h' % len(sigs))
EOF

echo "Building _HideUI.dll ..."
FLAGS=(-O2 -std=c++11 -municode -static-libgcc -static-libstdc++
       -Wall -Wextra -Wno-unused-parameter -I"$LUA_INC")
if [ -n "${HIDEUI_DEFINES:-}" ]; then
    read -ra defines <<< "$HIDEUI_DEFINES"
    FLAGS+=("${defines[@]}")
    echo "  defines: $HIDEUI_DEFINES"
fi

i686-w64-mingw32-g++ "${FLAGS[@]}" -c -o "$OBJ" "$ENGINE_DIR/hideui.cpp"

# Nothing of ours may run at process exit: the engine can be pinned for the
# session, and a static destructor runs at exit whatever DllMain does.
symbols=$(i686-w64-mingw32-nm "$OBJ")
if grep -qE '(^| )U .*(atexit|__cxa_atexit)' <<< "$symbols"; then
    echo "  FAIL: the engine registers something to run at process exit" >&2
    grep -E '(^| )U .*(atexit|__cxa_atexit)' <<< "$symbols" >&2
    rm -f "$OBJ"
    exit 1
fi
echo "  OK statics: nothing registered to run at process exit"

# --no-insert-timestamp keeps the link reproducible, so the deployed image can
# be named by its md5.
i686-w64-mingw32-g++ "${FLAGS[@]}" -shared \
    -o "$OUT" "$OBJ" "$ROOT/libLuaCore.a" \
    -lkernel32 -Wl,--enable-stdcall-fixup -Wl,--no-insert-timestamp
rm -f "$OBJ"

# grep -q closes the pipe early, which trips pipefail via SIGPIPE -- read the
# exports into a variable first.
exports=$(i686-w64-mingw32-objdump -p "$OUT")
if grep -q 'luaopen__HideUI' <<< "$exports"; then
    echo "  OK export: luaopen__HideUI"
else
    echo "  FAIL: luaopen__HideUI not exported" >&2
    exit 1
fi
file "$OUT" | grep -q 'PE32 executable' || { echo "  FAIL: not a PE32 image" >&2; exit 1; }

# The daemon is pinned for the life of a client once loaded: a mapped copy is
# never replaced, only left alone when it is already the same file.
DAEMON_OUT="$OUT_DIR/hideui_daemon.dll"
if cmp -s "$DAEMON" "$DAEMON_OUT"; then
    echo "  OK daemon: already the current build"
elif pid=$(mapped_by "$where/hideui_daemon.dll"); then
    echo "  FAIL: a different hideui_daemon.dll is mapped in pid $pid; exit that client first" >&2
    exit 1
else
    cp "$DAEMON" "$DAEMON_OUT"
    echo "  OK daemon: copied"
fi

echo "  $(md5sum "$OUT" | cut -d' ' -f1)  $OUT"
echo "  $(md5sum "$DAEMON_OUT" | cut -d' ' -f1)  $DAEMON_OUT"
echo "Done."
