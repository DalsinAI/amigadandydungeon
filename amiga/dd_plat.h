/* dd_plat.h: what Dandy Dungeon needs of the machine, in one place:
 * amiga/dd_acgame.c gives it on GFOS through ACGame (acgame.library),
 * tests/host/dd_host.c on a PC for the host tests.
 *
 * Apache-2.0, like Dandy Dungeon. Copyright (c) 2026 AmiCradle Team.
 */
#ifndef DD_PLAT_H
#define DD_PLAT_H

#ifdef __cplusplus
extern "C" {
#endif

#define DD_W 320
#define DD_H 200

/* the first pad, as bits */
#define DD_PAD_UP    1
#define DD_PAD_DOWN  2
#define DD_PAD_LEFT  4
#define DD_PAD_RIGHT 8
#define DD_PAD_FIRE  16     /* A: shoot */
#define DD_PAD_EAT   32     /* B: eat food */
#define DD_PAD_BOMB  64     /* X: smart bomb */
#define DD_PAD_QUIT  128    /* Start held with Back, or the window closed */

struct dd_input {
    unsigned char raw[128];     /* every key by its Amiga raw key code: 1 held */
    unsigned pad;               /* DD_PAD_* */
};

/* Before anything else: the game's drawer becomes the current directory. */
void dd_plat_start(int argc, char **argv);
/* The 320 x 200 256-colour screen. 0 when it is open. */
int dd_plat_open(void);
void dd_plat_close(void);
/* Show the frame: pixels (320 x 200, one byte each) with pal (256 x RGB). */
void dd_plat_show(const unsigned char *pixels, const unsigned char *pal);
void dd_plat_input(struct dd_input *in);
/* Milliseconds from any fixed point. */
unsigned long dd_plat_ms(void);
/* Nothing to do for ms milliseconds: give the time back. */
void dd_plat_idle(unsigned long ms);

#ifdef __cplusplus
}
#endif
#endif
