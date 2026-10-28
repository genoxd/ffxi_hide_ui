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

# The addresses below are the 2026-05-10 build's: table, manager, mask table,
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
        103712D0 105EDD10 10375294 10118FB0 1015E7A0 1015E1E0 1015F890 1015E570 1015E3C0 \
        10125160 10118100 1021E870 101A2F10 10222670 10123910 \
        1011A790 10117670 1011FB20 1000CD00 1011F1A0 \
        10205F50 100E9330 10202D10 101E3EB0 1020EA00 1020F020 100F7E40 1014CEB0 \
        1015F5F0 1011A440 10118D30 \
        101E41D6 10487B00 1014E30E 10484A68 \
        10118C50 100B8370 10488938 100B7FA1 1062F9F8 100B8380 10128390 \
        10118B50 10118BFA 1015DB90 1063335C 1062EC74 102108D0 1033AC30 \
        10210EA0 1012DEF3 101CBE80 10152DA0 10210960 1012DF40 1012DE40 \
        10210CB7 10140C90 1013A770 10140C30 1013A710 \
        1012EC20 101398C0 10151470 101D9030 101D9550 101DE6D0 1012E170 10139650 \
        10150E60 101DB4B0 101DE0E0 101E01A0 101EC7A0 101ECEC0 1020F8E0 1020FD90 \
        1015E620 10222250 102223B9 1022A68B 1022A890 1022A8E0 102221F7 10198C90 10193700 \
        101D6800 101D67B0 104DED90 3C432 101D6840 101FC890 101FC610 101F6BB0 101F6C30 101F7790 101F9F50 100A3C57 10486F08 10484A70 \
        10141875 1023BC40 \
        101D6070 10337460 101D5CF0 101D5DE0 101D5BC9 101CBC00 100F0F20 101CBBD0 101D5D50 100AD100 101D5CC0 \
        1062F6AC 1062F6A8 105EDDBE 10481F10 1037DC28 \
        10136A90 10136AD0 101266E0 10126930 10037E80 10129130 101268A0 10487200 10136B70 10136C79 104E1148 104E114C \
        1014F310 1014F3D0 1014F5B0 1014F3B0 101C5A60 1015DA10 \
        101DD990 10337E60 101DCE60 101DCCF0 1018F9C0 1018FB60 \
        101FEF10 101FE9F0 104DED90 1035F1C4 100DB640 10310BCE 100F6F40 100F6F00 100F6E80 100F7050 100F71E0 \
        101FF3D0 101FF450 105771F0 10220D50 1062FFA0 \
        1020DED0 1020DDC0 1063096C 10107580 10107530 1020A500 \
        101E4FC0 101E4C60 100E95D0 101E3000 101C8A90 100E9480 101CB2D0 1062F9EC 1062F9F0 1062F9F4 1062F9F5 \
        102062B0 10206020 10205F80 101DCBE0 101FE930 1062F278 101C63D0 101C63A0 \
        1015040A 10150446 \
        10151490 10151050 101511C0 101511F0 10151C40 10370D68 100EB9A0 100FC260 10151B60 10577480 1047FAE8 \
        101398E0 10139CD0 10139B70 10139CB0 105771EC 104DED90 194B8 19300 9860 41B4 104DED8C 43E2C 11E \
        100DBF20 10491E08 100EB330 101F10AA 105EDCE0 1015BAD0 1015A530 1AF 1062F94C 210 214 7C \
        10132FD0 10097C80 101AE330 1022AD00 \
        101F7340 101DEB50 102071A0 1014048E 101CBE80 101E3FA0 10141A40 101F2F7B 105EDCE0 1015B870 CB4 101CBC00 \
        101E0960 5C 101FCA90 101A6640 1062EC80 101A4000 \
        10145B10 10145A40 1025AF70 14 1062F6AC 1062F6B0 101EB190 10200710 101DEA40 10225F70 1014A3F0 1014AE10 \
        1014BA00 1020A130 58 1020D2F0 1020D440 1020D510 1020D5E0 1013E2B0 10 1012C070 1012C3A0 10144BB0 1C 14 68 \
        10221710 10221730 10230220 1022D6B0 1022D5B0 1038597C 70 100F9370 101FB440 104DED90 3C348 101E74B0 160 18 \
        14 1C 18 101A40F0 C 101A40C0 10150B70 1021C280 10633304 90 1062FC2C 101F1ED0 4 \
        101A4070 101F7870 10148A50 101983AD 100EB7E0 \
        10141A90 101E8640 \
        101936C0 1062EBA4 10195100 10195230 10378A20 10378B00 10378EF0 10378F60 10378FD0 10379040 10378B48 10378C28 \
        10378D08 10378DE8 10379120 10379210 101A7B60 100DBFE0 1062ECBC 2328 1019C030 100E4560 104DED90 2CFD0 \
        || status=1
else
    echo "== image_check skipped: $IMAGE is not there"
fi
exit $status
