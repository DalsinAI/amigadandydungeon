Dandy Dungeon 1.0 for GFOS
==========================

Dandy Dungeon: the dungeon game Jack Palevich wrote for the Atari
800 in 6502 assembly, in the C++ version he later wrote and released
under the Apache licence (version 2.0). One or two players walk
through 26 levels of mazes, shoot the ghosts and their generators, pick up money, food, keys
and smart bombs, open the locks, and take the stairs down. Level z leads
back to level a.

Keys
----
Player 1:
  W A S D (or the cursor keys)   walk
  Space                          shoot the way you are facing
  1                              eat food
  F1                             set off a smart bomb
Player 2:
  Keypad 8 4 5 6                 walk
  Keypad 0                       shoot
  2                              eat food
  F2                             set off a smart bomb
Both:
  F11                            go down a level (the game's own test key)
  Esc                            quit

A pad (GFOpenInput, or a joystick in port 2) is player 1: the stick or
d-pad walks, A shoots, B eats, X sets off a bomb, Start and Back
together quit.

What it needs
-------------
GFOS (AmigaOS 3.2 with GFOpenUp) on a 68040 or 68060; built soft float,
no FPU needed. The game draws on ACGame's 320 x 200 256-colour screen: on
RTG (and GFOpenGfx where it is installed) or AGA. It needs no SDL and no
GFOpenGPU. It ships acgame.library in the drawer's Libs/. Under 200 KB of
disk.

Sound: none. The game had none.

The game keeps nothing on disk: each start is level a.

Installing
----------
From the archive: double-click Install-DandyDungeon; it asks where the
drawer goes (Work:Games by default). With GFOpenUp:

  GFOpenUpTool INSTALL Parts/DandyDungeon

puts it in SYS:Games/DandyDungeon.

How it was ported
-----------------
The game's own code (Dandy.cpp) is kept: its map, levels, players,
ghosts, arrows and smart bombs, its key table and its scrolling. Its
Windows and Direct3D parts are replaced (amiga/patches/0001):

  - the view draws each cell's 16 x 16 tile from Dandy.bmp into ACGame's
    320 x 200 8-bit screen (amiga/dd_view.h, amiga/dd_draw.c), 20 cells
    across the 60-cell map, scrolling to keep both players in view;
  - the two status lines under the map are the port's own (Direct3D drew
    none): each player's health bar, food, keys, bombs and money, and the
    level letter as a number;
  - the Amiga's raw keys and the pad become the Windows key codes the
    game's key table reads (amiga/dd_keys.c);
  - the game moves on every 16 ms, as its Windows timer did.
