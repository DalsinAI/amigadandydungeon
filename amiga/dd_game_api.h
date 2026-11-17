/* dd_game_api.h: the calls amiga/dd_game.h gives into Dandy.cpp's game.
 * Apache-2.0, like Dandy Dungeon. Copyright (c) 2026 AmiCradle Team.
 */
#ifndef DD_GAME_API_H
#define DD_GAME_API_H

#ifdef __cplusplus
extern "C" {
#endif

/* the map's cells (Dandy.cpp's MapData) */
enum {
    DD_SPACE, DD_WALL, DD_LOCK, DD_UP, DD_DOWN, DD_KEY, DD_FOOD, DD_MONEY, DD_BOMB,
    DD_GHOST, DD_SMILEY, DD_BIG, DD_HEART, DD_GEN1, DD_GEN2, DD_GEN3,
    DD_ARROW0, DD_PLAYER0 = 24
};
#define DD_MAP_W 60
#define DD_MAP_H 30

struct dd_player_info {
    int x, y, health, food, keys, bombs, visible;
    unsigned long score;
};

/* level 1 (a), both players placed at its start */
void dd_game_start(void);
/* One step (1/60 s): vk[256] is every Windows virtual key held; then the
 * frame is drawn into dd_screen. */
void dd_game_step(const unsigned char *vk);
int dd_game_level(void);
int dd_game_load_level(int index);
int dd_game_cell(int x, int y);
void dd_game_set_cell(int x, int y, int v);
void dd_game_player(int i, struct dd_player_info *out);

/* amiga/dd_keys.c: Amiga raw key to the Windows virtual key the game's key
 * table names (0: none); the pad's bits as those keys for player 1 */
int dd_rawkey_to_vk(unsigned rawkey);
void dd_keys_from_input(const unsigned char *raw, unsigned pad, unsigned char *vk);

#ifdef __cplusplus
}
#endif
#endif
