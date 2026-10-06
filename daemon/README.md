# daemon

`hideui_daemon.dll`. The engine loads it once per game session and it stays
loaded until the game closes. Source: `hideui_daemon.cpp`, `hideui_abi.h`,
`exports.def`.

```sh
bash daemon/build.sh                  # needs i686-w64-mingw32-g++; writes hideui_daemon.dll here
bash tools/daemon_harness/build.sh    # its test harness, under wine
```
