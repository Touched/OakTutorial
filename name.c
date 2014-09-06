#include "engine/video.h"
#include "engine/text.h"
#include "engine/callback.h"
#include "engine/player.h"
#include "engine/input.h"
#include "engine/script.h"
#include "images/background.h"
#include "images/prof.h"
#include "images/rival.h"
#include "images/dawn.h"
#include "images/lucas.h"
#include "strings.h"

#define NAME_PLAYER 0
#define NAME_RIVAL 4

void npc_to_objtemplate(u8 id, u32 *callback, template *output, u32 *field14);
void name_screen(u8 type, u32 *dest, u16 image, u16 arg1, u32 arg2, u32 *callback);

// name_screen(NAME_PLAYER

typedef struct namingInstance {
	u8 prefilled; /* Whether the box is automatically filled */
	u8 length; /* The maximum characters a user can enter */
	u8 image_function_index; /* What function to use */
	enum {
		NONE,
		MALE,
		FEMALE
	} gender : 8; /* What gender symbol is displayed */
	u32 unk2; /* Always 1, doesn't appear to do anything */
	char *prompt; /* 0xFF terminated string */
} namingInstance;

typedef void (*nameLoadSprite)(void);

typedef struct overworld {
	u16 tiles_tag;
	u16 palette;
	u16 pal_tag;
	u16 field6;
	u16 x;
	u16 y;
	u8 pal_slot;
	u16 unused;
	sprite oam;
	u32 field14;
	frame ***animations;
	resource **graphics;
	u32 *rotscale;
} overworld;

void test(u8 ow_num) {
	overworld **overworlds = (overworld *) 0x0839FDB0;
	u32 value;
	template out = { 0 };


	npc_to_objtemplate(ow_num, (u32 *) 0x0800760C, &out, &value);
}

void preRepeatGender(u8 index);
void showNamePlayerScreen(u8 index);
void fadeForNamePlayerScreen(u8 index);
void returnFromPlayerName(u8 index);
void confirmPlayerName(u8 index);
void confirmPlayerNameHandler(u8 index);
void setPlayerName(u8 index);
void profIntroduceRival(u8 index);

void namePlayer(u8 index) {
	showMessage(caAskName);

	tasks[index].function = (u32)fadeForNamePlayerScreen;
}

void fadeForNamePlayerScreen(u8 index) {
	if (check_a_pressed(0)) return;

	void (*fade)() = (void (*)(void)) 0x08070588 + 1;

	/* We must use this fade so that the name screen can fade in again */
	fade();

	tasks[index].function = (u32) showNamePlayerScreen;
}

frame still[] = { { 0x0, 1 } , { 0xFFFF, 0x0 } };
frame still2[] = { { 0x40, 1 } , { 0xFFFF, 0x0 } };

/* Animation table */
frame *pAnimTopStatic[] = { still };
frame *pAnimBottomStatic[] = { still2 };

sprite oam = { 0x400, 0xC000, 0x800, 0x0 };

void static_callback(object *self) {
	self->final_oam.attr0 |= 0x400;
}

/* Prof */
resource profGraphics = { profTiles, 0x4000, 0x300 };
resource profPalette = { profPal, 0x300, 0x300  };
template profTop = { 0x300, 0x300, &oam, &pAnimTopStatic, 0, 0x8231CFC, static_callback };
template profBottom = { 0x300, 0x300, &oam, &pAnimBottomStatic, 0, 0x8231CFC, static_callback };

/* Rival */
resource rivalGraphics = { rivalTiles, 0x4000, 0x500 };
resource rivalPalette = { rivalPal, 0x500, 0x500  };
template rivalTop = { 0x500, 0x500, &oam, &pAnimTopStatic, 0, 0x8231CFC, static_callback };
template rivalBottom = { 0x500, 0x500, &oam, &pAnimBottomStatic, 0, 0x8231CFC, static_callback };

/* Lucas */
resource lucasGraphics = { lucasTiles, 0x4000, 0x1000 };
resource lucasPalette = { lucasPal, 0x1000, 0x1000  };
template lucasTop = { 0x1000, 0x1000, &oam, &pAnimTopStatic, 0, 0x8231CFC, static_callback };
template lucasBottom = { 0x1000, 0x1000, &oam, &pAnimBottomStatic, 0, 0x8231CFC, static_callback };

/* Dawn */
resource dawnGraphics = { dawnTiles, 0x4000, 0x200 };
resource dawnPalette = { dawnPal, 0x200, 0x200  };
template dawnTop = { 0x200, 0x200, &oam, &pAnimTopStatic, 0, 0x8231CFC, static_callback };
template dawnBottom = { 0x200, 0x200, &oam, &pAnimBottomStatic, 0, 0x8231CFC, static_callback };

void loadStaticSprite(resource *graphics, resource *palette, template *top, template *bottom, u16 x, u16 y) {
	object_load_compressed_graphics((u32*) graphics);
	object_load_palette((u32*) palette);

	object_display((u32*) top, x, y, 1);
	object_display((u32*) bottom, x, y + 0x40, 1);
}

/*
 * Yay! Macro pasting
 */
#define LOAD_STATIC_SPRITE(name, x,y) loadStaticSprite(&(name ## Graphics), &(name ## Palette), &(name ## Top), &(name ## Bottom), x, y)

void super_callback_shit();
void showNamePlayerScreen(u8 index) {
	/*
	 * Load a random default name into the player's name
	 */

	if (fade_control->color & 0x80)  {
		//name_screen(NAME_PLAYER, (*player)->name, (*player)->gender, 0, 0, 0x08130C64 + 1);
		name_screen(NAME_PLAYER, (*player)->name, (*player)->gender, 0, 0, super_callback_shit);
 	}

	((void (*)(u8, u8)) 0x08131169)(index, 1);

	variables[0xC] = 1;
}

void returnFromPlayerName(u8 index) {
	/*
	 * IO registers were cleared after return. Reset BLDCNT
	 */

	display_ioreg_set(0x50, 0x2F00);
	display_ioreg_set(0x52, 0x0F);

	if ((*player)->gender) {
		LOAD_STATIC_SPRITE(dawn, 0x77, 0x20);
	} else {
		LOAD_STATIC_SPRITE(lucas, 0x77, 0x20);
	}

	unfadeScreen();

	showMessage(caConfirmName);
	tasks[index].function = (u32) confirmPlayerName;
}

void confirmPlayerName(u8 index) {
	char *choices[] = { caYes, caNo, 0 };

	rbox box = {0, 2, 2, 4, 4, 0xF, 0x130};
	tasks[index].args[0xD] = showMultichoice(&box, choices);

	tasks[index].function = (u32) confirmPlayerNameHandler;
}

void confirmPlayerNameHandler(u8 index) {
	u16 id = tasks[index].args[0xD];
	int i = 0;

	switch(multichoice_get_response()) {
	case 0:
		/* Yes */
		tasks[index].function = (u32) setPlayerName;
		hideMultichoice(id);
		break;
	case 1:
		/* No */
		tasks[index].function = (u32) preRepeatGender;
		hideMultichoice(id);
		break;
	}
}

void setPlayerName(u8 index) {

}

void profIntroduceRival(u8 index) {
}


