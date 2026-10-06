#!/usr/bin/env bash
# Builds and runs the four test layers. Exits non-zero when any check fails,
# so it can gate a change.
#
#   engine_test.exe  core.h and game.h under wine, against fabricated menus
#   e2e_test.exe     the shipped _HideUI.dll and daemon, through Windower's own
#                    LuaCore.dll, against a stand-in FFXiMain.dll, under wine.
#                    The engine is copied into build/a and build/b, one
#                    basename in two folders as two addons ship it, which the
#                    loader maps as two images, and hideui.lua beside the one
#                    in build/a; build/c, build/d and build/e hold copies
#                    built from the same source that publish engine abi 2's,
#                    abi 3's and abi 4's tables, standing for a 0.4.x, a
#                    0.5.0 and a 0.5.1 to 0.6.3 resident.
#   lua_client_test  examples/hideuidemo's Lua under lua5.1, against a stand-in
#                    native module
#   image_check.exe  the signatures and the menu table against an unpacked
#                    FFXiMain.dll given as $FFXIMAIN_IMAGE, under wine; skipped
#                    when that is not set
#
# The end-to-end run needs the engine built first (../build.sh). It tests
# examples/hideuidemo/libs/_HideUI.dll, or $ENGINE_DLL, with the hideui_daemon.dll
# beside it, and reads LuaCore.dll from the FFXI plugins folder, or $LUACORE.
set -euo pipefail
cd "$(dirname "$0")"

FFXI="$(cd ../../.. && pwd)"
ROOT="$(cd ../.. && pwd)"
ADDON="$ROOT/examples/hideuidemo"
LUACORE="${LUACORE:-$FFXI/plugins/LuaCore.dll}"
ENGINE="${ENGINE_DLL:-$ADDON/libs/_HideUI.dll}"
DAEMON="$(dirname "$ENGINE")/hideui_daemon.dll"
FLAGS=(-std=c++11 -O2 -Wall -Wextra -Wno-unused-parameter -static-libgcc -static-libstdc++)

i686-w64-mingw32-g++ "${FLAGS[@]}" -static -o engine_test.exe engine_test.cpp
i686-w64-mingw32-g++ "${FLAGS[@]}" -shared -o FFXiMain.dll fake_ffximain.cpp
i686-w64-mingw32-g++ "${FLAGS[@]}" -static -o e2e_test.exe e2e_test.cpp
i686-w64-mingw32-g++ "${FLAGS[@]}" -static -o image_check.exe image_check.cpp

mkdir -p build/a build/b build/c build/d build/e
for copy in a b; do
    cp "$ENGINE" "build/$copy/_HideUI.dll"
    cp "$DAEMON" "build/$copy/hideui_daemon.dll"
done
cp "$ADDON/libs/hideui.lua" build/a/hideui.lua
# Engine abi 2's table ends at query_options, 140 bytes; abi 3's at status3,
# 236; abi 4's at status4, 284.
for table in "c 2 140" "d 3 236" "e 4 284"; do
    read -r copy abi bytes <<< "$table"
    if ! log=$(HIDEUI_DEFINES="-DHU_ENGINE_ABI_BUILT=${abi}u -DHU_ENGINE_PUBLISHED_SIZE=${bytes}u" \
            bash ../build.sh "$(pwd)/build/$copy/_HideUI.dll" 2>&1); then
        echo "$log"
        echo "FAIL  could not build the engine abi $abi table copy"
        exit 1
    fi
done

status=0
echo "== engine_test (wine)"
env -u DISPLAY WINEDEBUG="${WINEDEBUG:--all}" wine engine_test.exe || status=1
echo
echo "== e2e_test (wine): $(md5sum "$ENGINE" | cut -c1-12) engine as copies a and b, $(md5sum build/c/_HideUI.dll | cut -c1-12) engine abi 2 table as copy c, $(md5sum build/d/_HideUI.dll | cut -c1-12) engine abi 3 table as copy d, $(md5sum build/e/_HideUI.dll | cut -c1-12) engine abi 4 table as copy e, $(md5sum "$DAEMON" | cut -c1-12) daemon, $(md5sum "$LUACORE" | cut -c1-12) LuaCore"
env -u DISPLAY WINEDEBUG="${WINEDEBUG:--all}" wine e2e_test.exe \
    "Z:$LUACORE" "Z:$(pwd)/build/a/_HideUI.dll" "Z:$(pwd)/FFXiMain.dll" \
    "Z:$(pwd)/build/b/_HideUI.dll" "Z:$(pwd)/build/c/_HideUI.dll" "Z:$(pwd)/build/a/hideui.lua" \
    "Z:$(pwd)/build/d/_HideUI.dll" "Z:$(pwd)/build/e/_HideUI.dll" || status=1
echo
echo "== lua_client_test (lua5.1)"
lua5.1 lua_client_test.lua "$ADDON" || status=1

# The addresses below are the 2026-05-10 build's: table, manager, mask table,
# SetPosition, close, open, UI update, staged close, show path, the mouse mode
# picker, the menu input sink, the eleven called routines in signatures.h's
# kCalls order, then the link5 cache site and cache, the query cancel site and
# its cancel-allowed byte, SetCursor, the link5 latch site and latch, the link5
# open site, its controller global and its callback, the text-to-glyph
# converter, the row hit test and its menu+0x77 test, and the routing routine.
IMAGE="${FFXIMAIN_IMAGE:-}"
echo
if [ -z "$IMAGE" ]; then
    echo "== image_check skipped: FFXIMAIN_IMAGE is not set"
elif [ -f "$IMAGE" ]; then
    echo "== image_check (wine): $IMAGE $(md5sum "$IMAGE" | cut -c1-12)"
    env -u DISPLAY WINEDEBUG="${WINEDEBUG:--all}" wine image_check.exe "Z:$IMAGE" \
        103712D0 105EDD10 10375294 10118FB0 1015E7A0 1015E1E0 1015F890 1015E570 1015E3C0 \
        10125160 10118100 \
        10205F50 100E9330 10202D10 101E3EB0 1020EA00 1020F020 100F7E40 1014CEB0 \
        1015F5F0 1011A790 1011A440 \
        101E41D6 10487B00 1014E30E 10484A68 \
        10118C50 100B8370 10488938 100B7FA1 1062F9F8 100B8380 10128390 \
        10118B50 10118BFA 1015DB90 || status=1
else
    echo "== image_check skipped: $IMAGE is not there"
fi
exit $status
