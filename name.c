#include "engine/video.h"
#include "engine/text.h"
#include "engine/callback.h"
#include "engine/player.h"
#include "engine/input.h"
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

void showNamePlayerScreen(u8 index);
void fadeForNamePlayerScreen(u8 index);
void returnFromPlayerName(u8 index);

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

void super_callback_shit();
void showNamePlayerScreen(u8 index) {
	/*
	 * Load a random default name into the player's name
	 */

	if (fade_control->color & 0x80)  {
		//name_screen(NAME_PLAYER, (*player)->name, (*player)->gender, 0, 0, 0x08130C64 + 1);
		name_screen(NAME_PLAYER, (*player)->name, (*player)->gender, 0, 0, super_callback_shit);
 	}

	//tasks[index].function = (u32) returnFromPlayerName;
}

void returnFromPlayerName(u8 index) {
	//showMessage(caAskName);
}

