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
#                    in build/a; build/c, build/d, build/e, build/f,
#                    build/g, build/h, build/i and build/j hold copies built
#                    from the same source that publish engine abi 2's, abi
#                    3's, abi 4's, abi 5's, abi 6's, abi 7's, abi 8's and abi
#                    9's tables, the last up to set_setting9, standing for a
#                    0.4.x, a 0.5.0, a 0.5.1 to 0.6.3, a 0.7.0 to 0.8.0, a
#                    0.9.0, a 0.10.0 and two earlier 0.11.0 builds' residents.
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

mkdir -p build/a build/b build/c build/d build/e build/f build/g build/h build/i build/j
for copy in a b; do
    cp "$ENGINE" "build/$copy/_HideUI.dll"
    cp "$DAEMON" "build/$copy/hideui_daemon.dll"
done
cp "$ADDON/libs/hideui.lua" build/a/hideui.lua
# Engine abi 2's table ends at query_options, 140 bytes; abi 3's at status3,
# 236; abi 4's at status4, 284; abi 5's at block5, 328; abi 6's at macros,
# 340; abi 7's at unblock7, 348; abi 8's at withdraw8, 388; abi 9's at
# set_setting9, 408, before info9 and unblock9.
for table in "c 2 140" "d 3 236" "e 4 284" "f 5 328" "g 6 340" "h 7 348" "i 8 388" "j 9 408"; do
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
echo "== e2e_test (wine): $(md5sum "$ENGINE" | cut -c1-12) engine as copies a and b, $(md5sum build/c/_HideUI.dll | cut -c1-12) engine abi 2 table as copy c, $(md5sum build/d/_HideUI.dll | cut -c1-12) engine abi 3 table as copy d, $(md5sum build/e/_HideUI.dll | cut -c1-12) engine abi 4 table as copy e, $(md5sum build/f/_HideUI.dll | cut -c1-12) engine abi 5 table as copy f, $(md5sum build/g/_HideUI.dll | cut -c1-12) engine abi 6 table as copy g, $(md5sum build/h/_HideUI.dll | cut -c1-12) engine abi 7 table as copy h, $(md5sum build/i/_HideUI.dll | cut -c1-12) engine abi 8 table as copy i, $(md5sum build/j/_HideUI.dll | cut -c1-12) engine abi 9 table to set_setting9 as copy j, $(md5sum "$DAEMON" | cut -c1-12) daemon, $(md5sum "$LUACORE" | cut -c1-12) LuaCore"
env -u DISPLAY WINEDEBUG="${WINEDEBUG:--all}" wine e2e_test.exe \
    "Z:$LUACORE" "Z:$(pwd)/build/a/_HideUI.dll" "Z:$(pwd)/FFXiMain.dll" \
    "Z:$(pwd)/build/b/_HideUI.dll" "Z:$(pwd)/build/c/_HideUI.dll" "Z:$(pwd)/build/a/hideui.lua" \
    "Z:$(pwd)/build/d/_HideUI.dll" "Z:$(pwd)/build/e/_HideUI.dll" "Z:$(pwd)/build/f/_HideUI.dll" \
    "Z:$(pwd)/build/g/_HideUI.dll" "Z:$(pwd)/build/h/_HideUI.dll" "Z:$(pwd)/build/i/_HideUI.dll" \
    "Z:$(pwd)/build/j/_HideUI.dll" || status=1
echo
echo "== lua_client_test (lua5.1)"
lua5.1 lua_client_test.lua "$ADDON" || status=1

# The addresses below are the 2026-11-10 build's: table, manager, mask table,
# SetPosition, close, open, UI update, staged close, show path, the mouse mode
# picker, the menu input sink, the compass draw entry, the macro key gate, the
# ability opener, the key test, the template swap, the per-menu draw, the
# sprite draw, the draw primitive, the quad routine that calls it, the eleven
# called routines in signatures.h's kCalls order,
# then the link5 cache
# site and cache, the query cancel site and its cancel-allowed byte, SetCursor,
# the link5 latch site and latch, the link5 open site, its controller global
# and its callback, the text-to-glyph converter, the row hit test and its
# menu+0x77 test, the routing routine, the compass global, the macro object
# global, the yes/no class's constructor and its vtable, its confirm, the KO
# window's confirm site, the decider, the message getter, the yes/no ask and
# the callback that site names, the KO window's tick, the yes/no class's
# test of its cancellable byte, the number boxes' confirms and setups, the
# sixteen askers' callbacks in signatures.h's kAskerSites order, then close by
# instance and the Asked windows' sites and what they lead to: sortyn's setup
# and confirm, the key item list's ask, evitem01's setup, callback and input
# routine, confyn's input routine and restore, bankmenu's open hook, clear,
# player data global, Mog Safe 2 flag offset and input routine, maplist's
# close+reset, rebuild, select and cursor read, map0's input routine and the
# map's close, the GM tell poller, its slot
# global and queue, mp_dead's and
# ok's input routines; mogdoor's input routine, vtable, open hook, draw, the
# door's handler, the confirm's gate, the floor read, the draw's condition,
# the tick, the door actor's open, the routine the exit gives the door, the
# Mog House object's global, the exit's three bytes and the nations' lists;
# the dialogue boxes' input routine, handler, text dispatch, its gate's
# advance and page-turn sound, the chat log's release, the text object's
# clear, the event's wait flag, the tick and its left-button test's return,
# and the two globals naming their text objects that the dispatch hands its
# code between; the text-entry windows:
# stringdl's input routine, name and slot handlers and field end, a field's
# text set and the manager's focus;
# comment's input routine, vtable, begin-close, field end, the line's
# sanitize and trim; msgline's input routine, field end, the player data
# global, the POL interface's global and number, the clock's text, the
# senders of modes 0, 1, 2, 5 and 7, their two callbacks, the sending
# indicator's global and mark, the busy byte; scname's input routine, field
# end, the player search's global, the criteria's clear and name, the
# search's start; link11's input routine, field end, the second slot's
# test, link5's selection, the name's encoder, the request's sender, the
# item lookup and the request's four bytes; passinpu's input routine, field
# end and start; comment's and msgline's field starts; the text entry's
# manager's global, its unbind and its bind; then where the target
# window's two arrow draws return; then the trade windows: the trade's input
# routine, stage, OK, cancel, close, rows' slots, send and the allocator,
# the trade's open, its mode's global and the actor table; the NPC trade's
# input routine, stage, OK, close and mode's global; the player's data's
# global, the own and the partner's offers', the inventory's and the last
# sends' offsets, the intervals' global and offset, the packet ids the
# throttle takes, the clock and its object's global, the item record, the
# inventory list's rows, the item cache's global, the item's data and
# description, the lock byte's offset, the mode listing the modal statuses,
# and the actor's titled name's length, titled name and name offsets; then
# the equipment set's opener, the Monstrosity check, the equipment shortcut
# and the magic list's opener; then the map's opener, the blue magic set's,
# the automaton's, the main menu's linkshell row, its check and the
# linkshell list's opener, the instincts' opener, the item use, its item
# cache's global, item data lookup and type offset, its check, the
# synthesis's opener and the offset of inventor's list it keeps, the maze
# editor's opener, the macro set menu's input
# routine, the macro editor's global and its opener; then the routes: the
# storage list's opener, the post menu's, the job list's key lookup,
# jobcselu's row offset, the Mog House's and the Nomad Moogle service's
# manager globals, the friend list's, the mail list's and the blacklist's
# openers, the mount list's, the Job Points test and opener, fp_cat2's
# opener, the search reset, the results' mode offset, comgenre's opener and
# the three comment sub-lists', the item panel's set-and-open and the
# record's extra data offset, the two timers' openers, mp_stat's opener,
# persona's layout, conf11l's set and quest00's mode offsets, quest00's
# confirm's two calls and quest01's setup, the missions menu's setter, the
# expansion test, the logs' expansion bits, miss00's log offset, the map's
# tracking and scanlist's opener, the player data's global and the area's
# offset, the Region Info opener and cnqframe's kind offset, coloresu's
# region and the three text color offsets, the macro pick and the palette
# offset, the macro editor's close, titlewin's step back, the item view,
# its global, inventor's Items byte offset, the Items row's list's global,
# inventor's open with a list, and the item summary's type offset; then the
# macro palette switch, the Wide Scan question's callback, statcom2's
# begin-close, textcol1's Escape and the bag's size; then mp_mncst's Escape
# and cnqframe's input routine; then the settings: the config store's set,
# its global, its set method and its save, the chat filter's, the battle
# effects filter's, the chat log routing's four and the text colors' four
# tables, k1assign's and k2assign's rows, the equipment lock's request, its
# wait's clock, its stamp's global and its wait in milliseconds, the idle
# timer, the player flag's request, the player data's global and the player
# flags' offset.
IMAGE="${FFXIMAIN_IMAGE:-}"
echo
if [ -z "$IMAGE" ]; then
    echo "== image_check skipped: FFXIMAIN_IMAGE is not set"
elif [ -f "$IMAGE" ]; then
    echo "== image_check (wine): $IMAGE $(md5sum "$IMAGE" | cut -c1-12)"
    env -u DISPLAY WINEDEBUG="${WINEDEBUG:--all}" wine image_check.exe "Z:$IMAGE" \
        10373320 106229F0 10377368 1011A5A0 1015FEE0 1015F920 10160FD0 1015FCB0 1015FB00 \
        10126750 101196F0 102203A0 101A49B0 102241A0 10124F00 \
        1011BD80 10118C60 10121110 1000CD00 10120790 \
        10207A30 100E95D0 102047F0 101E5950 102104E0 10210B00 100F80E0 1014E5D0 \
        10160D30 1011BA30 1011A320 \
        101E5C76 10489CB8 1014FA2E 10486C20 \
        1011A240 100B8450 1048AAF0 100B8081 106646D8 100B8460 10129980 \
        1011A140 1011A1EA 1015F2D0 1066803C 10663954 102123B0 1033CD78 \
        10212980 1012F453 101CD920 101544C0 10212440 1012F4A0 1012F3A0 \
        10212797 101421F0 1013BCD0 10142190 1013BC70 \
        10130180 1013AE20 10152B90 101DAAD0 101DAFF0 101E0170 1012F6D0 1013ABB0 \
        10152580 101DCF50 101DFB80 101E1C40 101EE240 101EE960 102113C0 10211870 \
        1015FD60 10223D80 10223EE9 1022C1BB 1022C3C0 1022C410 10223D27 1019A740 10194E40 \
        101D82A0 101D8250 104E0F48 3C4EA 101D82E0 101FE370 101FE0F0 101F8690 101F8710 101F9270 101FBA30 100A3D37 104890C0 10486C28 \
        10142DD5 1023D8A0 \
        101D7B10 103395A8 101D7790 101D7880 101D7669 101CD6A0 100F11C0 101CD670 101D77F0 100AD1E0 101D7760 \
        1066438C 10664388 10622A9E 104840C8 1037FCF8 \
        10137FF0 10138030 10127CD0 10127F20 10037F50 1012A720 10127E90 104893B8 101380D0 101381D9 104E3308 104E330C \
        10150A30 10150AF0 10150CD0 10150AD0 101C7500 1015F150 \
        101DF430 10339FA8 101DE900 101DE790 10191100 101912A0 \
        102009F0 102004D0 104E0F48 103611BC 100DB780 1031280E 100F71E0 100F71A0 100F7120 100F72F0 100F7480 \
        10200EB0 10200F30 105793B0 10222880 10664C80 \
        1020F9B0 1020F8A0 1066564C 10107830 101077E0 1020BFE0 \
        101E6A60 101E6700 100E9870 101E4AA0 101CA530 100E9720 101CCD70 106646CC 106646D0 106646D4 106646D5 \
        10207D90 10207B00 10207A60 101DE680 10200410 10663F58 101C7E70 101C7E40 \
        10151B2A 10151B66 \
        10152BB0 10152770 101528E0 10152910 10153360 10372DA8 100EBC40 100FC510 10153280 10579644 10481CA0 \
        1013AE40 1013B230 1013B0D0 1013B210 105793AC 104E0F48 194C0 19308 9868 41B4 104E0F44 43EE4 120 \
        100DC060 10493FC0 100EB5D0 101F2B4A 106229C0 1015D210 1015BC50 1B3 1066462C 210 214 7C \
        10134530 10097D50 101AFDD0 1022C830 \
        101F8E20 101E05F0 10208C80 101419EE 101CD920 101E5A40 10142FA0 101F4A1B 106229C0 1015CFB0 14B4 101CD6A0 \
        101E2400 5C 101FE570 101A80E0 10663960 101A5AA0 \
        10147070 10146FA0 1025CBD0 14 1066438C 10664390 101ECC30 102021F0 101E04E0 10227AA0 1014B9C0 1014C3E0 \
        1014D120 1020BC10 58 1020EDD0 1020EF20 1020EFF0 1020F0C0 1013F810 10 1012D5D0 1012D900 10146110 1C 14 68 \
        10223240 10223260 10231D50 1022F1E0 1022F0E0 10387A4C 70 100F9610 101FCF20 104E0F48 3C400 101E8F50 160 18 \
        14 1C 18 101A5B90 C 101A5B60 10152290 1021DDB0 10667FE4 90 1066490C 101F3970 4 \
        101A5B10 101F9350 1014A080 10199E5D 100EBA80 \
        10142FF0 101EA0E0 \
        10194E00 10663884 10196890 101969C0 1037AAF0 1037ABD0 1037AFC0 1037B030 1037B0A0 1037B110 1037AC18 1037ACF8 \
        1037ADD8 1037AEB8 1037B1F0 1037B2E0 101A9600 100DC120 1066399C 2328 1019DAE0 100E4710 104E0F48 2CFD8 \
        || status=1
else
    echo "== image_check skipped: $IMAGE is not there"
fi
exit $status
