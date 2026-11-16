/* dd_draw.c: see dd_draw.h.
 * Apache-2.0, like Dandy Dungeon. Copyright (c) 2026 AmiCradle Team.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dd_draw.h"

static unsigned rd16(const unsigned char *p) { return p[0] | (p[1] << 8); }
static unsigned long rd32(const unsigned char *p)
{
    return p[0] | (p[1] << 8) | ((unsigned long)p[2] << 16) | ((unsigned long)p[3] << 24);
}

int dd_colour(struct dd_frame *f, int r, int g, int b)
{
    int i, best = 0;
    long bestd = 0x7fffffffL;
    for (i = 0; i < f->colours; i++) {
        const unsigned char *c = f->pal + i * 3;
        long dr = c[0] - r, dg = c[1] - g, db = c[2] - b;
        long d = dr * dr + dg * dg + db * db;
        if (d <= 3 * 8 * 8) return i;  /* the BMP's near twins (46,55,174 and 47,55,174) are one */
        if (d < bestd) { bestd = d; best = i; }
    }
    if (f->colours < 256) {
        unsigned char *c = f->pal + f->colours * 3;
        c[0] = (unsigned char)r; c[1] = (unsigned char)g; c[2] = (unsigned char)b;
        return f->colours++;
    }
    return best;
}

int dd_load_tiles(struct dd_frame *f, const unsigned char *bmp, unsigned long size)
{
    unsigned long off, w, bpp, stride, x, y;
    long h;
    int flip;
    const unsigned char *pal = 0;
    if (size < 54 || bmp[0] != 'B' || bmp[1] != 'M') return -1;
    off = rd32(bmp + 10);
    w = rd32(bmp + 18);
    h = (long)rd32(bmp + 22);
    bpp = rd16(bmp + 28);
    if (rd32(bmp + 30) != 0) return -1;         /* compressed */
    flip = h > 0;
    if (h < 0) h = -h;
    if (w != 256 || h != 32 || (bpp != 24 && bpp != 8)) return -1;
    stride = ((w * bpp / 8) + 3) & ~3ul;
    if (off + stride * (unsigned long)h > size) return -1;
    if (bpp == 8) {
        pal = bmp + 14 + rd32(bmp + 14);
        if ((unsigned long)(pal - bmp) + 1024 > size) return -1;
    }
    for (y = 0; y < (unsigned long)h; y++) {
        const unsigned char *row = bmp + off + stride * (flip ? (unsigned long)h - 1 - y : y);
        for (x = 0; x < w; x++) {
            int r, g, b, tile, idx;
            if (bpp == 24) {
                b = row[x * 3]; g = row[x * 3 + 1]; r = row[x * 3 + 2];
            } else {
                const unsigned char *c = pal + row[x] * 4;
                b = c[0]; g = c[1]; r = c[2];
            }
            idx = dd_colour(f, r, g, b);
            tile = (int)(y / DD_CELL) * 16 + (int)(x / DD_CELL);
            f->tiles[tile][(y % DD_CELL) * DD_CELL + x % DD_CELL] = (unsigned char)idx;
        }
    }
    return 0;
}

int dd_load_tiles_file(struct dd_frame *f, const char *path)
{
    FILE *in = fopen(path, "rb");
    unsigned char *buf;
    long n;
    int r;
    if (!in) return -1;
    fseek(in, 0, SEEK_END);
    n = ftell(in);
    fseek(in, 0, SEEK_SET);
    if (n <= 0 || n > 1 << 20 || !(buf = (unsigned char *)malloc((size_t)n))) { fclose(in); return -1; }
    r = fread(buf, 1, (size_t)n, in) == (size_t)n ? dd_load_tiles(f, buf, (unsigned long)n) : -1;
    free(buf);
    fclose(in);
    return r;
}

void dd_clear(struct dd_frame *f, int colour, int y0, int y1)
{
    if (y0 < 0) y0 = 0;
    if (y1 > 200) y1 = 200;
    if (y1 > y0) memset(f->pixels + y0 * 320, colour, (size_t)(y1 - y0) * 320);
}

void dd_fill(struct dd_frame *f, int x, int y, int w, int h, int colour)
{
    int i, j;
    for (j = y; j < y + h; j++)
        for (i = x; i < x + w; i++)
            if (i >= 0 && i < 320 && j >= 0 && j < 200) f->pixels[j * 320 + i] = (unsigned char)colour;
}

void dd_draw_tile(struct dd_frame *f, int tile, int x, int y, int clip)
{
    int i, j;
    const unsigned char *t;
    if (tile < 0 || tile >= DD_TILES) return;
    t = f->tiles[tile];
    for (j = 0; j < DD_CELL; j++) {
        int py = y + j;
        if (py < 0 || py >= clip || py >= 200) continue;
        for (i = 0; i < DD_CELL; i++) {
            int px = x + i;
            if (px >= 0 && px < 320) f->pixels[py * 320 + px] = t[j * DD_CELL + i];
        }
    }
}

/* 3 x 5 digits, a row in 3 bits each */
static const unsigned short digits[10] = {
    075557, 022222, 071747, 071717, 055711, 074717, 074757, 071111, 075757, 075717
};

int dd_draw_number(struct dd_frame *f, unsigned long n, int x, int y, int colour)
{
    char s[12];
    int k, i, j;
    sprintf(s, "%lu", n);
    for (k = 0; s[k]; k++) {
        unsigned short d = digits[s[k] - '0'];
        for (j = 0; j < 5; j++)
            for (i = 0; i < 3; i++)
                if (d & (1 << ((4 - j) * 3 + (2 - i))))
                    dd_fill(f, x + i * 2, y + j * 2, 2, 2, colour);
        x += 8;
    }
    return x;
}
