/* dd_game.h: the game object, seen from outside Dandy.cpp
 * (amiga/patches/0001 includes this after "Game gGame;", so it sees the
 * game's classes; amiga/dd_main.c and the tests call it through
 * amiga/dd_game_api.h).
 *
 * Apache-2.0, like Dandy Dungeon. Copyright (c) 2026 AmiCradle Team.
 */
#ifndef DD_GAME_H
#define DD_GAME_H

#include "dd_game_api.h"

extern "C" {

void dd_game_start(void)
{
	gGame.Start();
}

void dd_game_step(const unsigned char *vk)
{
	for (int k = 0; k < Keyboard::KeySize; k++)
		gGame.HandleEvent(vk[k] != 0, (UCHAR)k);
	gGame.Step();
	gGame.Render();
}

int dd_game_level(void)
{
	return gGame.world.level;
}

int dd_game_load_level(int index)
{
	gGame.world.LoadLevel((DWORD)index);
	return gGame.world.level;
}

int dd_game_cell(int x, int y)
{
	if (x < 0 || y < 0 || x >= (int)Map::Width || y >= (int)Map::Height) return -1;
	return gGame.world.map.Cell[x + y * Map::Width];
}

void dd_game_set_cell(int x, int y, int v)
{
	gGame.world.map.Set((DWORD)x, (DWORD)y, v);
}

void dd_game_player(int i, struct dd_player_info *out)
{
	Player *p = &gGame.world.player[i];
	out->x = p->x;
	out->y = p->y;
	out->health = p->health;
	out->food = p->food;
	out->keys = p->keys;
	out->bombs = p->bombs;
	out->score = p->score;
	out->visible = p->IsVisible();
}

}

#endif
