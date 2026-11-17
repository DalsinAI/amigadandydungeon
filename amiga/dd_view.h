/* dd_view.h: Dandy.cpp's View class on our frame instead of Direct3D
 * (amiga/patches/0001 puts this where upstream's View was; it sees the
 * game's classes above it). The same cells as upstream's: the 20 x 10 cells
 * round the players' centre, scrolling a pixel at a time, 16 x 16 each;
 * then a status line below them (upstream had none).
 *
 * Apache-2.0, like Dandy Dungeon. Copyright (c) 2026 AmiCradle Team.
 */
#ifndef DD_VIEW_H
#define DD_VIEW_H

#include "dd_draw.h"
#include "dd_plat.h"

extern "C" {
extern struct dd_frame dd_screen;
extern int dd_black, dd_white, dd_red, dd_blue;
}

class View
{
public:
	void Render(World& world)
	{
		struct dd_frame *f = &dd_screen;
		float cogX, cogY;
		DWORD startX, endX, startY, endY;
		world.GetCOG(cogX, cogY);
		world.map.GetActive(cogX, cogY, startX, startY, endX, endY);
		/* upstream: xBase = -cogX * 16 - 0.5, a cell's left edge at xBase + x * 16 */
		int xBase = -(int)(cogX * 16.f + 0.5f);
		int yBase = -(int)(cogY * 16.f + 0.5f);
		dd_clear(f, dd_black, 0, DD_VIEW_H);
		for (DWORD y = startY; y < endY; y++)
			for (DWORD x = startX; x < endX; x++)
				dd_draw_tile(f, world.map.Get(x, y), xBase + (int)x * DD_CELL,
				             yBase + (int)y * DD_CELL, DD_VIEW_H);
		Status(world);
	}

	void Status(World& world)
	{
		struct dd_frame *f = &dd_screen;
		dd_clear(f, dd_black, DD_VIEW_H, DD_H);
		dd_fill(f, 0, DD_VIEW_H, DD_W, 1, dd_blue);
		for (DWORD i = 0; i < world.numPlayers && i < 2; i++)
		{
			Player *p = &world.player[i];
			int y = DD_VIEW_H + 3 + (int)i * 18;
			int x = 2;
			dd_draw_tile(f, kPlayer0 + (int)i, x, y, DD_H);
			x += 18;
			/* health: nine blocks */
			for (int h = 0; h < Player::kHealthMax; h++)
				dd_fill(f, x + h * 5, y + 4, 4, 8, h < p->health ? dd_red : dd_blue);
			x += Player::kHealthMax * 5 + 6;
			dd_draw_tile(f, kFood, x, y, DD_H);
			x = dd_draw_number(f, p->food, x + 17, y + 3, dd_white) + 4;
			dd_draw_tile(f, kKey, x, y, DD_H);
			x = dd_draw_number(f, p->keys, x + 17, y + 3, dd_white) + 4;
			dd_draw_tile(f, kBomb, x, y, DD_H);
			x = dd_draw_number(f, p->bombs, x + 17, y + 3, dd_white) + 4;
			dd_draw_tile(f, kMoney, x, y, DD_H);
			dd_draw_number(f, p->score, x + 17, y + 3, dd_white);
		}
		/* the level, 1 to 26 */
		dd_draw_tile(f, kDown, DD_W - 42, DD_VIEW_H + 3, DD_H);
		dd_draw_number(f, (unsigned long)world.level + 1, DD_W - 24, DD_VIEW_H + 6, dd_white);
	}
};

#endif
