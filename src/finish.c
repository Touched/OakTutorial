#include "engine/video.h"
#include "engine/text.h"
#include "engine/callback.h"
#include "engine/player.h"
#include "engine/input.h"
#include "engine/script.h"
#include "engine/math.h"
#include "../images/lucasshrink.h"
#include "../images/dawnshrink.h"
#include "global.h"
#include "../strings.h"

void shrinkPlayer(u8 index);
void fadeToBlack(u8 index);
void startGame(u8 index);

#define FRAME_DURATION 0x15

frame shrink[] = { { 0x0, FRAME_DURATION }, { 0x80, FRAME_DURATION }, { 0x100, FRAME_DURATION }, { 0x180, FRAME_DURATION }, { 0xFFFF, 0 } };
frame shrink2[] = { { 0x40, FRAME_DURATION }, { 0xC0, FRAME_DURATION }, { 0x140, FRAME_DURATION }, { 0x1C0, FRAME_DURATION }, { 0xFFFF, 0 } };

/* Animation table */
frame *pAnimTopShrink[] = { shrink };
frame *pAnimBottomShrink[] = { shrink2 };

sprite oam = { 0x400, 0xC000, 0x800, 0x0 };

void static_callback(object *self) {
}

/* Lucas */
resource lucasGraphics = { (void *) lucasshrinkTiles, 0x4000, 0x1000 };
resource lucasPalette = { (void *) lucasshrinkPal, 0x1000, 0x1000  };
template lucasTop = { 0x1000, 0x1000, &oam, pAnimTopShrink, 0, ROTSCALE_EMPTY, static_callback };
template lucasBottom = { 0x1000, 0x1000, &oam, pAnimBottomShrink, 0, ROTSCALE_EMPTY, static_callback };

/* Dawn */
resource dawnGraphics = { (void *) dawnshrinkTiles, 0x4000, 0x200 };
resource dawnPalette = { (void *) dawnshrinkPal, 0x200, 0x200  };
template dawnTop = { 0x200, 0x200, &oam, pAnimTopShrink, 0, ROTSCALE_EMPTY, static_callback };
template dawnBottom = { 0x200, 0x200, &oam, pAnimBottomShrink, 0, ROTSCALE_EMPTY, static_callback };

void loadShrinkSprite(resource *graphics, resource *palette, template *top, template *bottom, u16 x, u16 y) {
	object_load_compressed_graphics(graphics);
	object_load_palette(palette);

	object_display(top, x, y, 1);
	object_display(bottom, x, y + 0x40, 1);
}

/*
 * Moar macro pasting!
 */

#define LOAD_SHRINK_SPRITE(name) loadShrinkSprite(&(name ## Graphics), &(name ## Palette), &(name ## Top), &(name ## Bottom), 0x77, 0x20)

void fadeInPlayerLastTime(u8 index) {
	u16 bg = tasks[index].args[7];
	u16 fg = tasks[index].args[8];

	if (fg < 0x10) {
		if (!tasks[index].args[6]) {
			tasks[index].args[7] = bg - 1;
			tasks[index].args[8] = fg + 1;
			display_ioreg_set(0x52, (bg << 8) | fg);

			tasks[index].args[6] = FADE_STEPS;
		} else {
			tasks[index].args[6] -= 1;
		}

	} else {
		int i = 0;
		for (i = 0; i < 2; ++i)
			object_delete_and_free(&objects[i]);

		if ((*player)->gender)
			LOAD_SHRINK_SPRITE(dawn);
		else
			LOAD_SHRINK_SPRITE(lucas);

		tasks[index].args[6] = FRAME_DURATION * 4; /* Four frames */
		tasks[index].function = (u32) shrinkPlayer;
	}
}

void shrinkPlayer(u8 index) {
	if (!tasks[index].args[6]) {
		tasks[index].function = (u32) fadeToBlack;
	}
	tasks[index].args[6] -= 1;
}

void fadeToBlack(u8 index) {
	fadescreen(0xFFFFFFFF, 0x2, 0, 0x10, 0x0000);
	tasks[index].function = (u32) startGame;
}

void startGame(u8 index) {
	/*
	 * Wait for fade
	 */
	if (fade_control->color & 0x80) return;

	/*
	 * Hand over to the new game callback.
	 */

	tasks[index].function = (u32) 0x08130915;
}

