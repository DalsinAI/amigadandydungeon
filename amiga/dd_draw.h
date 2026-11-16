/* dd_draw.h: Dandy Dungeon's picture, drawn into a 320 x 200 8-bit frame:
 * the tiles (upstream's Dandy.bmp: 16 x 2 cells of 16 x 16, read once into
 * the frame's palette), the 20 x 10 cell view, and the status line under it
 * (Dandy.cpp had none: each player's health, food, keys, bombs and score,
 * and the level).
 *
 * Apache-2.0, like Dandy Dungeon. Copyright (c) 2026 AmiCradle Team.
 */
#ifndef DD_DRAW_H
#define DD_DRAW_H

#ifdef __cplusplus
extern "C" {
#endif

#define DD_CELL 16
#define DD_TILES 32
#define DD_VIEW_H 160

struct dd_frame {
    unsigned char pixels[320 * 200];
    unsigned char pal[256 * 3];
    unsigned char tiles[DD_TILES][DD_CELL * DD_CELL];
    int colours;            /* palette entries used */
};

/* Read a 24-bit (or 8-bit) uncompressed BMP of 256 x 32 into the tiles,
 * adding its colours to the palette. 0 when it is read. */
int dd_load_tiles(struct dd_frame *f, const unsigned char *bmp, unsigned long size);
/* The same from a file (a \ in the name is a /). */
int dd_load_tiles_file(struct dd_frame *f, const char *path);
/* The palette index for r, g, b: one within 8 a channel of it, or a new
 * one while there is room, or the nearest. */
int dd_colour(struct dd_frame *f, int r, int g, int b);

void dd_clear(struct dd_frame *f, int colour, int y0, int y1);
/* A tile at x, y (may be partly outside the view: clipped to y < clip). */
void dd_draw_tile(struct dd_frame *f, int tile, int x, int y, int clip);
/* Digits 0-9 in a 3 x 5 font, scale 2: returns the x after them. */
int dd_draw_number(struct dd_frame *f, unsigned long n, int x, int y, int colour);
void dd_fill(struct dd_frame *f, int x, int y, int w, int h, int colour);

#ifdef __cplusplus
}
#endif
#endif
