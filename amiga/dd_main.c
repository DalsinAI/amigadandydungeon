/* dd_main.c: Dandy Dungeon's main loop, on amiga/dd_plat.h: the tiles are
 * read (Dandy.bmp), level 1 starts, then 60 times a second the keys and
 * the pad are read as the game's virtual keys, the game takes a step and
 * draws, and the frame is shown. Esc ends it (upstream closed its window).
 *
 * Apache-2.0, like Dandy Dungeon. Copyright (c) 2026 AmiCradle Team.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dd_plat.h"
#include "dd_draw.h"
#include "dd_game_api.h"

#define DD_STEP_MS 16       /* upstream stepped once a frame, 60 a second */

struct dd_frame dd_screen;
int dd_black, dd_white, dd_red, dd_blue;
unsigned long dd_debug_breaks;
static unsigned long start_ms;

unsigned long dd_ticks(void)
{
    return dd_plat_ms() - start_ms;
}

FILE *dd_fopen(const char *path, const char *mode)
{
    char buf[260];
    size_t i;
    for (i = 0; path[i] && i + 1 < sizeof(buf); i++)
        buf[i] = path[i] == '\\' ? '/' : path[i];
    buf[i] = 0;
    return (fopen)(buf, mode);
}

int dd_setup(void)
{
    memset(&dd_screen, 0, sizeof(dd_screen));
    dd_black = dd_colour(&dd_screen, 0, 0, 0);
    if (dd_load_tiles_file(&dd_screen, "Dandy.bmp") != 0 &&
        dd_load_tiles_file(&dd_screen, "dandy.bmp") != 0) {
        printf("Dandy Dungeon: Dandy.bmp is missing or not a 256 x 32 picture\n");
        return -1;
    }
    dd_white = dd_colour(&dd_screen, 255, 255, 255);
    dd_red = dd_colour(&dd_screen, 201, 99, 99);
    dd_blue = dd_colour(&dd_screen, 46, 55, 174);
    return 0;
}

int main(int argc, char **argv)
{
    struct dd_input in;
    unsigned char vk[256];
    unsigned long next;
    dd_plat_start(argc, argv);
    if (dd_setup() != 0) return 20;
    if (dd_plat_open() != 0) return 20;
    start_ms = dd_plat_ms();
    dd_game_start();
    next = dd_plat_ms();
    for (;;) {
        unsigned long now;
        dd_plat_input(&in);
        if (in.raw[0x45] || (in.pad & DD_PAD_QUIT)) break;     /* Esc */
        dd_keys_from_input(in.raw, in.pad, vk);
        dd_game_step(vk);
        dd_plat_show(dd_screen.pixels, dd_screen.pal);
        next += DD_STEP_MS;
        now = dd_plat_ms();
        if ((long)(next - now) > 0) dd_plat_idle(next - now);
        else if ((long)(now - next) > 200) next = now;          /* fell behind: don't race */
    }
    dd_plat_close();
    return 0;
}
