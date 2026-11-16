/* dd_acgame.c: amiga/dd_plat.h on GFOS, through ACGame (acgame.library):
 * the game's 320 x 200 screen is ACGame's 8-bit screen (an RTG mode, drawn
 * by GFOpenGfx where it is installed, or AGA; no SDL and no GFOpenGPU),
 * its palette ACGame's, the keyboard ACGame's raw key table, and the first
 * pad (GFOpenInput, or lowlevel.library's port 2) the DD_PAD_* bits:
 * stick and d-pad move, A shoots, B eats, X bombs, Start with Back quits.
 * The pointer is hidden: the game has none.
 *
 * The game's files are read from its own drawer (PROGDIR:), wherever it is
 * started from.
 *
 * Apache-2.0, like Dandy Dungeon. Copyright (c) 2026 AmiCradle Team.
 */
#include <stdio.h>
#include <string.h>

#include <exec/types.h>
#include <dos/dos.h>
#include <proto/dos.h>

#include <acgame/platform.h>

#include "dd_plat.h"

unsigned long __stack = 32 * 1024;

static BPTR progdir, olddir;
static int have_olddir, screen;

void dd_plat_start(int argc, char **argv)
{
    (void)argc; (void)argv;
    progdir = Lock((CONST_STRPTR)"PROGDIR:", ACCESS_READ);
    if (progdir) {
        olddir = CurrentDir(progdir);
        have_olddir = 1;
    }
}

static void restore_dir(void)
{
    if (have_olddir) {
        CurrentDir(olddir);
        UnLock(progdir);
        have_olddir = 0;
    }
}

int dd_plat_open(void)
{
    acgame_video_config cfg;
    cfg.width = DD_W;
    cfg.height = DD_H;
    cfg.refresh_hz = 60;
    cfg.mode = ACGAME_VIDEO_INDEXED8;
    cfg.display = ACGAME_DISPLAY_AUTO;
    if (acgame_platform_init(&cfg) != 0) {
        printf("Dandy Dungeon: ACGame could not open a 320 x 200 256-colour screen\n");
        restore_dir();
        return -1;
    }
    acgame_platform_show_pointer(0);
    screen = 1;
    return 0;
}

void dd_plat_close(void)
{
    if (screen) acgame_platform_shutdown();
    screen = 0;
    restore_dir();
}

void dd_plat_show(const unsigned char *pixels, const unsigned char *pal)
{
    acgame_indexed_surface *s;
    int y, i;
    if (!screen) return;
    s = acgame_platform_begin_indexed_frame();
    if (!s) return;
    for (y = 0; y < DD_H && y < (int)s->height; y++)
        memcpy(s->pixels + (size_t)y * s->pitch, pixels + y * DD_W, DD_W);
    for (i = 0; i < 256; i++) {
        s->palette[i].r = pal[i * 3];
        s->palette[i].g = pal[i * 3 + 1];
        s->palette[i].b = pal[i * 3 + 2];
    }
    acgame_platform_present_indexed(s);
}

#define PAD_DEAD 12000

void dd_plat_input(struct dd_input *in)
{
    acgame_input_state st;
    const uint8_t *k;
    uint32_t b;
    memset(in, 0, sizeof(*in));
    if (!screen) return;
    acgame_platform_poll(&st);
    k = acgame_platform_rawkeys();
    if (k) memcpy(in->raw, k, sizeof(in->raw));
    if (!st.pad.connected) return;
    b = st.pad.buttons;
    if ((b & ACGAME_PAD_BIT(ACGAME_PAD_DPAD_UP)) || st.pad.axes[ACGAME_PAD_LEFTY] < -PAD_DEAD) in->pad |= DD_PAD_UP;
    if ((b & ACGAME_PAD_BIT(ACGAME_PAD_DPAD_DOWN)) || st.pad.axes[ACGAME_PAD_LEFTY] > PAD_DEAD) in->pad |= DD_PAD_DOWN;
    if ((b & ACGAME_PAD_BIT(ACGAME_PAD_DPAD_LEFT)) || st.pad.axes[ACGAME_PAD_LEFTX] < -PAD_DEAD) in->pad |= DD_PAD_LEFT;
    if ((b & ACGAME_PAD_BIT(ACGAME_PAD_DPAD_RIGHT)) || st.pad.axes[ACGAME_PAD_LEFTX] > PAD_DEAD) in->pad |= DD_PAD_RIGHT;
    if (b & ACGAME_PAD_BIT(ACGAME_PAD_A)) in->pad |= DD_PAD_FIRE;
    if (b & ACGAME_PAD_BIT(ACGAME_PAD_B)) in->pad |= DD_PAD_EAT;
    if (b & ACGAME_PAD_BIT(ACGAME_PAD_X)) in->pad |= DD_PAD_BOMB;
    if ((b & ACGAME_PAD_BIT(ACGAME_PAD_START)) && (b & ACGAME_PAD_BIT(ACGAME_PAD_BACK))) in->pad |= DD_PAD_QUIT;
}

unsigned long dd_plat_ms(void)
{
    return acgame_platform_ticks_ms();
}

void dd_plat_idle(unsigned long ms)
{
    /* a whole 50 Hz tick or more to wait: give it back to the system */
    if (ms >= 20) Delay(ms / 20);
}
