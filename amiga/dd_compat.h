/* dd_compat.h: what Dandy.cpp took from <Windows.h> (amiga/patches/0001
 * puts this in its place): the types, the virtual key codes its key table
 * names, GetTickCount, min and max (Visual C's macros), and DebugBreak (it
 * is called where a map read is out of bounds: here it counts them, for the
 * tests). File names keep their \ (levels\level.a): fopen turns it into /.
 *
 * Apache-2.0, like Dandy Dungeon. Copyright (c) 2026 AmiCradle Team.
 */
#ifndef DD_COMPAT_H
#define DD_COMPAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned long DWORD;
typedef unsigned char BYTE;
typedef unsigned char UCHAR;

#define MAX_PATH 260

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

/* the Windows virtual keys the game's key table uses (letters and digits
 * are their ASCII capitals) */
#define VK_SPACE   0x20
#define VK_NUMPAD0 0x60
#define VK_NUMPAD4 0x64
#define VK_NUMPAD5 0x65
#define VK_NUMPAD6 0x66
#define VK_NUMPAD8 0x68
#define VK_F1      0x70
#define VK_F2      0x71
#define VK_F11     0x7A

#ifdef __cplusplus
extern "C" {
#endif
extern unsigned long dd_debug_breaks;
#define DebugBreak() ((void)dd_debug_breaks++)

unsigned long dd_ticks(void);
#define GetTickCount() dd_ticks()

FILE *dd_fopen(const char *path, const char *mode);
#ifdef __cplusplus
}
#endif
#define fopen(p, m) dd_fopen((p), (m))

#endif
