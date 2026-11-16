/* dd_keys.c: the Amiga keyboard and the first pad, as the Windows virtual
 * keys Dandy.cpp's key table reads:
 *   player 1: W A S D move (or the cursor keys), Space shoots, 1 eats,
 *             F1 sets off a smart bomb;
 *   player 2: keypad 8 4 5 6 move, keypad 0 shoots, 2 eats, F2 bombs;
 *   F11 goes down a level (upstream's key for trying levels).
 * The pad is player 1: stick or d-pad moves, A shoots, B eats, X bombs.
 *
 * Apache-2.0, like Dandy Dungeon. Copyright (c) 2026 AmiCradle Team.
 */
#include <string.h>

#include "dd_plat.h"
#include "dd_game_api.h"

#define VK_SPACE   0x20
#define VK_NUMPAD0 0x60
#define VK_NUMPAD4 0x64
#define VK_NUMPAD5 0x65
#define VK_NUMPAD6 0x66
#define VK_NUMPAD8 0x68
#define VK_F1      0x70
#define VK_F2      0x71
#define VK_F11     0x7A

int dd_rawkey_to_vk(unsigned rawkey)
{
    static const char row1[] = "1234567890";        /* raw 0x01.. */
    static const char rowq[] = "QWERTYUIOP";        /* raw 0x10.. */
    static const char rowa[] = "ASDFGHJKL";         /* raw 0x20.. */
    static const char rowz[] = "ZXCVBNM";           /* raw 0x31.. */
    if (rawkey >= 0x01 && rawkey <= 0x0a) return row1[rawkey - 0x01];
    if (rawkey >= 0x10 && rawkey <= 0x19) return rowq[rawkey - 0x10];
    if (rawkey >= 0x20 && rawkey <= 0x28) return rowa[rawkey - 0x20];
    if (rawkey >= 0x31 && rawkey <= 0x37) return rowz[rawkey - 0x31];
    switch (rawkey) {
    case 0x40: return VK_SPACE;
    case 0x0f: return VK_NUMPAD0;
    case 0x2d: return VK_NUMPAD4;
    case 0x2e: return VK_NUMPAD5;
    case 0x2f: return VK_NUMPAD6;
    case 0x3e: return VK_NUMPAD8;
    case 0x50: return VK_F1;
    case 0x51: return VK_F2;
    case 0x4b: return VK_F11;       /* F11 on a PC-style keyboard */
    case 0x4c: return 'W';          /* cursor keys: player 1 */
    case 0x4d: return 'S';
    case 0x4e: return 'D';
    case 0x4f: return 'A';
    }
    return 0;
}

void dd_keys_from_input(const unsigned char *raw, unsigned pad, unsigned char *vk)
{
    unsigned k;
    memset(vk, 0, 256);
    for (k = 0; k < 128; k++)
        if (raw[k]) {
            int v = dd_rawkey_to_vk(k);
            if (v) vk[v] = 1;
        }
    if (pad & DD_PAD_UP) vk['W'] = 1;
    if (pad & DD_PAD_DOWN) vk['S'] = 1;
    if (pad & DD_PAD_LEFT) vk['A'] = 1;
    if (pad & DD_PAD_RIGHT) vk['D'] = 1;
    if (pad & DD_PAD_FIRE) vk[VK_SPACE] = 1;
    if (pad & DD_PAD_EAT) vk['1'] = 1;
    if (pad & DD_PAD_BOMB) vk[VK_F1] = 1;
}
