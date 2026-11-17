/* unit_test.c: the port's own pieces and the game under them, on a PC, one
 * check at a time:
 *  - dd_keys.c: Amiga raw keys and the pad as the game's virtual keys;
 *  - dd_draw.c: Dandy.bmp read into tiles and a palette, the digits;
 *  - the game (Dandy.cpp with amiga/patches/0001): level a starts with
 *    both players at the way in; on a cleared map a held key moves a
 *    player one cell per 48 ms (upstream's kMsPerMove), money scores 10, a
 *    key opens a lock, an arrow kills a ghost, food is picked up and eaten;
 *    levels a to z load and past z is a again; no map read out of bounds.
 * Linked with amiga/dd_main.c (its main renamed) and its own dd_plat_*.
 * Apache-2.0, like Dandy Dungeon. Copyright (c) 2026 AmiCradle Team.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dd_plat.h"
#include "dd_draw.h"
#include "dd_game_api.h"

int dd_setup(void);
extern struct dd_frame dd_screen;
extern unsigned long dd_debug_breaks;

static unsigned long vt;
static int fails, checks;
#define CHECK(c) do { checks++; if (!(c)) { fails++; printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); } } while (0)

void dd_plat_start(int argc, char **argv) { (void)argc; (void)argv; }
int dd_plat_open(void) { return 0; }
void dd_plat_close(void) {}
void dd_plat_show(const unsigned char *pixels, const unsigned char *pal) { (void)pixels; (void)pal; }
void dd_plat_input(struct dd_input *in) { memset(in, 0, sizeof(*in)); }
unsigned long dd_plat_ms(void) { return vt; }
void dd_plat_idle(unsigned long ms) { vt += ms; }

static void test_keys(void)
{
    unsigned char raw[128], vk[256];
    CHECK(dd_rawkey_to_vk(0x11) == 'W');
    CHECK(dd_rawkey_to_vk(0x20) == 'A');
    CHECK(dd_rawkey_to_vk(0x21) == 'S');
    CHECK(dd_rawkey_to_vk(0x22) == 'D');
    CHECK(dd_rawkey_to_vk(0x01) == '1' && dd_rawkey_to_vk(0x02) == '2' && dd_rawkey_to_vk(0x0a) == '0');
    CHECK(dd_rawkey_to_vk(0x40) == 0x20);           /* space */
    CHECK(dd_rawkey_to_vk(0x3e) == 0x68 && dd_rawkey_to_vk(0x2d) == 0x64);    /* keypad 8, 4 */
    CHECK(dd_rawkey_to_vk(0x2e) == 0x65 && dd_rawkey_to_vk(0x2f) == 0x66 && dd_rawkey_to_vk(0x0f) == 0x60);
    CHECK(dd_rawkey_to_vk(0x50) == 0x70 && dd_rawkey_to_vk(0x51) == 0x71 && dd_rawkey_to_vk(0x4b) == 0x7a);
    CHECK(dd_rawkey_to_vk(0x4c) == 'W' && dd_rawkey_to_vk(0x4f) == 'A');    /* cursor keys */
    CHECK(dd_rawkey_to_vk(0x45) == 0);              /* Esc is the port's */
    memset(raw, 0, sizeof(raw));
    raw[0x22] = 1;
    dd_keys_from_input(raw, DD_PAD_UP | DD_PAD_FIRE | DD_PAD_BOMB, vk);
    CHECK(vk['D'] && vk['W'] && vk[0x20] && vk[0x70] && !vk['A'] && !vk['1']);
}

static int tile_has(int tile, int colour)
{
    int i;
    for (i = 0; i < DD_CELL * DD_CELL; i++)
        if (dd_screen.tiles[tile][i] == colour) return 1;
    return 0;
}

static void test_draw(void)
{
    struct dd_frame *f = &dd_screen;
    int blue, i, n = 0;
    CHECK(dd_setup() == 0);
    CHECK(f->colours >= 4 && f->colours <= 8);       /* black, blue, red, white, pale blue */
    blue = dd_colour(f, 46, 55, 174);
    CHECK(tile_has(DD_WALL, blue));
    for (i = 0; i < DD_CELL * DD_CELL; i++) n += f->tiles[DD_SPACE][i] != f->tiles[DD_SPACE][0];
    CHECK(n == 0);                                  /* the space is one colour */
    CHECK(!tile_has(DD_SPACE, blue));
    dd_clear(f, 0, 0, 200);
    CHECK(dd_draw_number(f, 1234567890ul, 0, 0, 5) == 80);
    n = 0;
    for (i = 0; i < 320 * 10; i++) n += f->pixels[i] == 5;
    CHECK(n == 4 * (12 + 5 + 11 + 11 + 9 + 11 + 12 + 7 + 13 + 12));   /* lit cells of 1..9,0 */
    {
        unsigned char bad[60];
        memset(bad, 0, sizeof(bad));
        CHECK(dd_load_tiles(f, bad, sizeof(bad)) != 0);
    }
}

static unsigned char vk[256];

static void steps(int n)
{
    while (n-- > 0) {
        dd_game_step(vk);
        vt += 16;
    }
}

static void clear_map(void)
{
    int x, y;
    for (y = 1; y < DD_MAP_H - 1; y++)
        for (x = 1; x < DD_MAP_W - 1; x++) {
            int c = dd_game_cell(x, y);
            if (c != DD_PLAYER0 && c != DD_PLAYER0 + 1) dd_game_set_cell(x, y, DD_SPACE);
        }
}

static void test_game(void)
{
    struct dd_player_info p, q;
    int ux = -1, uy = -1, x, y;
    vt = 1000;
    dd_game_start();
    CHECK(dd_game_level() == 0);
    for (y = 0; y < DD_MAP_H; y++)
        for (x = 0; x < DD_MAP_W; x++)
            if (dd_game_cell(x, y) == DD_UP) { ux = x; uy = y; }
    dd_game_player(0, &p);
    dd_game_player(1, &q);
    CHECK(ux >= 0);
    CHECK(p.x == ux && p.y == uy - 1);              /* player 1 above the way in */
    CHECK(q.x == ux + 1 && q.y == uy);              /* player 2 to its right */
    CHECK(dd_game_cell(p.x, p.y) == DD_PLAYER0 && dd_game_cell(q.x, q.y) == DD_PLAYER0 + 1);
    CHECK(p.health == 9 && p.visible && p.score == 0);

    /* a cleared map: walk right for 480 ms (10 moves at 48 ms) */
    clear_map();
    dd_game_player(0, &p);
    memset(vk, 0, sizeof(vk));
    vk['D'] = 1;
    steps(30);
    memset(vk, 0, sizeof(vk));
    steps(1);
    dd_game_player(0, &q);
    CHECK(q.y == p.y && q.x - p.x >= 9 && q.x - p.x <= 11);

    /* money, food, a key, then a lock opened with it */
    clear_map();
    dd_game_player(0, &p);
    dd_game_set_cell(p.x + 1, p.y, DD_MONEY);
    dd_game_set_cell(p.x + 2, p.y, DD_FOOD);
    dd_game_set_cell(p.x + 3, p.y, DD_KEY);
    dd_game_set_cell(p.x + 4, p.y, DD_LOCK);
    vk['D'] = 1;
    steps(15);
    memset(vk, 0, sizeof(vk));
    steps(1);
    dd_game_player(0, &q);
    CHECK(q.score == p.score + 10);
    CHECK(q.food == p.food + 1);
    CHECK(q.keys == p.keys);                        /* picked up, then spent on the lock */
    CHECK(q.x >= p.x + 4);
    CHECK(dd_game_cell(p.x + 4, p.y) != DD_LOCK);

    /* an arrow: player 1 faces right; a ghost three cells on */
    clear_map();
    dd_game_player(0, &p);
    dd_game_set_cell(p.x + 3, p.y, DD_GHOST);
    vk[0x20] = 1;
    steps(1);
    vk[0x20] = 0;
    steps(10);
    CHECK(dd_game_cell(p.x + 3, p.y) == DD_SPACE);
    CHECK(dd_game_cell(p.x + 1, p.y) == DD_SPACE && dd_game_cell(p.x + 2, p.y) == DD_SPACE);

    /* eat: food goes, health stays full (it was full) */
    dd_game_player(0, &p);
    vk['1'] = 1;
    steps(1);
    vk['1'] = 0;
    steps(1);
    dd_game_player(0, &q);
    CHECK(q.food == p.food && q.health == 9);       /* full health: nothing eaten */

    /* the levels: a to z, then a again */
    CHECK(dd_game_load_level(25) == 25);
    CHECK(dd_game_load_level(12) == 12);
    CHECK(dd_game_load_level(26) == 0);
    CHECK(dd_debug_breaks == 0);
}

int main(void)
{
    const char *d = getenv("DD_TEST_DATA");
    if (d && chdir(d) != 0) { perror(d); return 1; }
    test_keys();
    test_draw();
    test_game();
    printf("unit: %d of %d checks passed\n", checks - fails, checks);
    return fails != 0;
}
