# AmigaDandyDungeon

Dandy Dungeon (Jack Palevich; Apache-2.0) for GFOS (AmigaOS 3.2
with GFOpenUp) on a 68040 or 68060: the one- or two-player dungeon game,
from its C++ remake, on ACGame's 320 x 200 256-colour screen
(acgame.library, opened at run time through its stubs and shipped in the
drawer), with no SDL and no GFOpenGPU. Installed as a GFOpenUp part:
`SYS:Games/DandyDungeon`.

Status: builds and plays on a PC from the same patched sources (see
Tests); the Amiga build is made by the stove, not yet run in a lab. The
game has no sound.

AMIGA_PORT.md is the drawer's ReadMe: the keys, what it needs, and how the
port works.

## Building

    STOVE=~/AmiCradle/stoves/os32-gcc16-open6 OUT=out sh package/build-amiga.sh

or in Cradle, stove_build with the os32-gcc16-open6 stove, target
`package`. Upstream (jackpal/Dandy-Dungeon) and DalsinAI/acgame are
fetched at the commits `package/dd.lock` pins; nothing of upstream is kept
here. `OUT` gets the archive (`DandyDungeon-1.0-gfos1-68040.lha`, the
drawer and Install-DandyDungeon) and `Parts/DandyDungeon`, installed with

    GFOpenUpTool INSTALL Parts/DandyDungeon

## Tests

    sh package/prepare-source.sh /tmp/dd && DD=/tmp/dd sh tests/host/test_dd.sh

on a PC:

- `tests/host/unit_test.c`: raw keys and the pad as the game's keys;
  Dandy.bmp read into tiles and a palette, the digits; the game itself:
  level a starts with both players at the way in, a held key moves a
  player one cell at a time, money, food, keys and locks, an arrow kills
  a ghost, levels a to z load and z leads to a, no debug breaks (39
  checks);
- the whole game on `tests/host/dd_host.c` (a frame in memory, virtual
  time, scripted keys and pad): level a is drawn with its status lines;
  player 1 walks with the keys and shoots; the same walk with the pad's
  d-pad ends on the same cell, and Start + Back quits.

The data files are read a byte at a time (levels as 4-bit cells, the BMP
by its bytes), so nothing in them depends on the CPU's byte order.

## Layout

- `amiga/dd_main.c`: the game's loop, its tiles and colours.
  `amiga/dd_view.h`: the view, drawing the map and the status lines.
  `amiga/dd_draw.c`: tiles, digits. `amiga/dd_keys.c`: raw keys and the
  pad to Windows key codes. `amiga/dd_compat.h`: the Windows types and
  calls the game used.
- `amiga/dd_acgame.c`: the screen, keys and pad on ACGame.
  `tests/host/dd_host.c`: on a PC.
- `amiga/patches/0001`: the game's own code, where it had to change.
- `package/`: the lock, the source preparation, the stove build, the part.

Designed, tested and delivered by The AmiCradle Team.
