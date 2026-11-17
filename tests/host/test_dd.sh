#!/bin/sh
# Host tests: the port's own pieces and the game under them
# (tests/host/unit_test.c), then the whole game built for this PC from the
# same patched dandy.cpp and amiga/dd_*.c, on tests/host/dd_host.c (a frame
# in memory, virtual time, scripted keys and pad):
#  - start: level a drawn with both players at the way in, the status line
#    under the map;
#  - keys: player 1 walks right (D) then down (S) and fires (space); the
#    frames differ and player 1 has moved;
#  - pad: the same walk with the pad's d-pad, then Start + Back ends the game.
#   DD=/path/to/prepared (package/prepare-source.sh DIR) sh tests/host/test_dd.sh
#   DD_TEST_KEEP=DIR keeps the frames (PPM) there.
# Apache-2.0, like Dandy Dungeon. Copyright (c) 2026 AmiCradle Team.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT=$(CDPATH= cd -- "$HERE/../.." && pwd)
DD=${DD:?DD=prepared source (package/prepare-source.sh DIR)}
CC=${HOSTCC:-cc}
CXX=${HOSTCXX:-c++}
T=$(mktemp -d "${TMPDIR:-/tmp}/dd-host.XXXXXX")
trap 'rm -rf "$T"' EXIT INT TERM

MINE="-std=gnu99 -Wall -Werror -O1 -I$ROOT/amiga"
UP="-std=gnu++11 -w -fno-exceptions -fno-rtti -O1 -I$ROOT/amiga"
$CXX $UP -c -o "$T/dandy.o" "$DD/src/dandy.cpp"
for f in dd_keys dd_draw; do $CC $MINE -c -o "$T/$f.o" "$ROOT/amiga/$f.c"; done
$CC $MINE -Dmain=dd_real_main -c -o "$T/dd_main_t.o" "$ROOT/amiga/dd_main.c"
$CC $MINE -c -o "$T/dd_main.o" "$ROOT/amiga/dd_main.c"
$CC $MINE -c -o "$T/unit_test.o" "$HERE/unit_test.c"
$CC $MINE -c -o "$T/dd_host.o" "$HERE/dd_host.c"
$CXX -o "$T/unit" "$T/unit_test.o" "$T/dd_main_t.o" "$T/dd_keys.o" "$T/dd_draw.o" "$T/dandy.o"
$CXX -o "$T/dd" "$T/dd_main.o" "$T/dd_keys.o" "$T/dd_draw.o" "$T/dandy.o" "$T/dd_host.o"

DD_TEST_DATA="$DD/data" "$T/unit"

fail=0
summary() { sed -n "s/^$2 //p" "$1/summary.txt"; }
differ() { ! cmp -s "$1" "$2"; }

run() {   # run NAME VAR=VALUE...: the game in its own drawer until DD_END
    n=$1; shift
    mkdir -p "$T/$n"
    if ! env DD_DATA="$DD/data" DD_OUT="$T/$n" "$@" "$T/dd" > "$T/$n/stdout.txt" 2> "$T/$n/stderr.txt"; then
        echo "FAIL $n: the game stopped"; tail -30 "$T/$n/stdout.txt" "$T/$n/stderr.txt"; fail=1; return 1
    fi
    [ -f "$T/$n/summary.txt" ] || { echo "FAIL $n: no summary"; fail=1; return 1; }
}
pos() { summary "$1" player1 | cut -d' ' -f1,2; }

if run start DD_END=1000 DD_SHOTS=500; then
    [ "$(summary "$T/start" opened)" = 1 ] || { echo "FAIL start: the screen was not opened once"; fail=1; }
    [ "$(summary "$T/start" shows)" -gt 30 ] || { echo "FAIL start: few frames shown"; fail=1; }
    [ "$(summary "$T/start" level)" = 0 ] || { echo "FAIL start: not level a"; fail=1; }
    c=$(summary "$T/start" colours); [ "$c" -ge 5 ] || { echo "FAIL start: $c colours"; fail=1; }
    [ "$(summary "$T/start" breaks)" = 0 ] || { echo "FAIL start: the game hit a debug break"; fail=1; }
    echo "start: level a drawn, $c colours, players at $(pos "$T/start")"
fi

# D (0x22) held 600 ms, S (0x21) held 600 ms, space (0x40) taps
KEYS="1000:34:1,1600:34:0,1700:33:1,2300:33:0,2500:64:1,2550:64:0,2800:64:1,2850:64:0"
if run keys DD_END=3500 DD_KEYS="$KEYS" DD_SHOTS=900,1650,2400,3400; then
    differ "$T/keys/shot-900.ppm" "$T/keys/shot-1650.ppm" || { echo "FAIL keys: walking right changed nothing"; fail=1; }
    differ "$T/keys/shot-1650.ppm" "$T/keys/shot-2400.ppm" || { echo "FAIL keys: walking down changed nothing"; fail=1; }
    [ "$(pos "$T/keys")" != "$(pos "$T/start")" ] || { echo "FAIL keys: player 1 did not move"; fail=1; }
    [ "$(summary "$T/keys" breaks)" = 0 ] || { echo "FAIL keys: the game hit a debug break"; fail=1; }
    echo "keys: player 1 walked from $(pos "$T/start") to $(pos "$T/keys")"
fi

# pad: right (8) then down (2), then Start+Back (128) quits before DD_END
if run pad DD_END=60000 DD_PAD="1000:8,1600:0,1700:2,2300:0,3000:128" DD_SHOTS=900,2400; then
    [ "$(pos "$T/pad")" = "$(pos "$T/keys")" ] || { echo "FAIL pad: the d-pad walk ended at $(pos "$T/pad"), the keys' at $(pos "$T/keys")"; fail=1; }
    v=$(summary "$T/pad" vt); [ "$v" -lt 60000 ] || { echo "FAIL pad: Start + Back did not end the game"; fail=1; }
    echo "pad: the d-pad walk matches the keys', Start + Back ended the game at $v ms"
fi

if [ -n "${DD_TEST_KEEP:-}" ]; then mkdir -p "$DD_TEST_KEEP"; cp -R "$T/start" "$T/keys" "$T/pad" "$DD_TEST_KEEP/"; fi
[ $fail = 0 ] && echo "host tests: all passed"
exit $fail
