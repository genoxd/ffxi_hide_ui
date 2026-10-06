# engine

`_HideUI.dll`, the module `hideui.lua` loads. Source: `hideui.cpp`, `core.h`,
`game.h`, `signatures.h`, `menu_table.inc`, `hideui_engine_abi.h`.

Build (needs `i686-w64-mingw32-g++` and `../libLuaCore.a`, the import library
for Windower's `LuaCore.dll`: the Lua 5.1 C API the engine links against):

```sh
bash engine/build.sh                   # writes examples/hideuidemo/libs/_HideUI.dll
bash engine/build.sh _HideUI.new.dll   # beside a copy the game has loaded
```

Tests (need `wine`, `lua5.1`, `python3`, and Windower's `plugins/LuaCore.dll`
as `$LUACORE` for the end-to-end suite; an unpacked `FFXiMain.dll` as
`$FFXIMAIN_IMAGE` runs the signature check, skipped without it):

```sh
bash engine/tests/build.sh
ENGINE_DLL=$PWD/examples/hideuidemo/libs/_HideUI.new.dll bash engine/tests/build.sh
```
