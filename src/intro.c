#include "../strings.h"
#include "../images/prof.h"
#include "engine/video.h"
#include "engine/callback.h"
#include "engine/text.h"
#include "engine/input.h"
#include "engine/script.h"
#include "global.h"

/*
 * All measurements are given in tiles
 */

#define SCREEN_WIDTH 32
#define SCREEN_HEIGHT 20
#define SCREEN_SIZE SCREEN_WIDTH * SCREEN_HEIGHT

#define PROF_HEIGHT 16
#define PROF_WIDTH 8
#define PROF_X 11
#define PROF_Y 0
#define PROF_MAP_BASE 0x618

void fadeInProfBg(u8 index);
void profIntroduce(u8 index);
void profAdvice(u8 index);
void profAdviceHandler(u8 index);
void profSlideRight(u8 index);
void profSlideLeft(u8 index);
void profThisWorld(u8 index);
void pokeballRoll(u8 index);
void pokeballOpen(u8 index);
void releaseTheBunny(u8 index);
void waitForJumpingFinish(u8 index);
void freePokeBall(u8 index);
void fadeBunny(u8 index);
void explainYourself(u8 index);
void profFadeOutAndFree(u8 index);
void chooseGender(u8 index);
void areYouABoyOrGirl(u8 index);

typedef struct {
	u16 SrcNum;				// Source Data Byte Size
	u8  SrcBitNum;			// 1 Source Data Bit Number
	u8  DestBitNum;			// 1 Destination Data Bit Number
	u32 DestOffset:31;		// Number added to Source Data
	u32 DestOffset0_On:1;	// Flag to add/not add Offset to 0 Data
} BUP;
void bitUnPack(void *source, void *dest, BUP* bup);

void startTutorial(u8 index) {
	showMessage(caHelloThere);

	tasks[index].function = (u32) fadeInProfBg;
}

void fadeInProfBg(u8 index) {
	if (check_a_pressed(0)) return;
	
	audio_play(0x124);

	/*
	 * Tilemap dimensions
	 */
	u8 *tilemap;
	u16 i, j, k;
	u32 size;
	void *data;
	BUP config = { 0 };

	/*
	 * Allocated enough space for a full screen tilemap
	 */
	tilemap = (u8*) malloc_and_clear(SCREEN_SIZE);
	gmemset(tilemap, 0xFF, SCREEN_SIZE);

	/*
	 * Generate the tilemap on the fly
	 */
	for (k = PROF_MAP_BASE, i = PROF_Y; i < PROF_Y + PROF_HEIGHT; ++i) {
		for (j = PROF_X; j < PROF_X + PROF_WIDTH; ++j)
			tilemap[i * SCREEN_WIDTH + j] = k++;
	}

	/*
	 * Unpack the professor graphics
	 */
	data = malloc_and_LZ77UnComp((void *) profTiles, &size);
	config.SrcNum = size;

	/*
	 * Upscale 4bpp to 8bpp
	 */
	config.SrcBitNum = 4;
	config.DestBitNum = 8;

	/*
	 * Select palette 4
	 */
	config.DestOffset = 64;

	/*
	 * Copy professor graphics to the map base of BG2
	 */
	bitUnPack(data, (void *) 0x06000600, &config);
	gpu_pal_apply((void *) profPal, 0x40, 0x40);
	free(data);

	bgid_set_tilemap(2, tilemap);
	bgid_send_tilemap(2);

	/*
	 * Slow unfade
	 */
	fadescreen(0xFFFFFFFF, 0x10, 0x10, 0x0, 0x0000);
	gpu_sync_bg_show(1);
	gpu_sync_bg_show(2);
	textbox_close();

	tasks[index].args[6] = 0xA0;
	tasks[index].function = (u32) profIntroduce;
}

void profIntroduce(u8 index) {
	/*
	 * Wait for fade
	 */
	if (fade_control->color & 0x80) return;

	if (tasks[index].args[6]) {
		tasks[index].args[6] -= 1;
		return;
	}

	showMessage(caIntroduce);
	tasks[index].function = (u32) profSlideRight;
}

void profSlideRight(u8 index) {
	if (check_a_pressed(0)) return;

	/*
	 * Slide right until he is positioned correctly
	 */
	if (bgid_get_x_offset(2) > -0x3600) {
		bgid_mod_x_offset(2, 0x200, 2);
		return;
	}

	tasks[index].function = (u32) profAdvice;
}

void profAdvice(u8 index) {
	if (check_a_pressed(0)) return;

	/*
	 * Show the advice menu
	 */

	char *choices[] = { caOptionInfoControls, caOptionInfoAdventure, caOptionInfoNone, 0 };
	rbox box = {0, 2, 2, 12, 6, 0xF, 0x130};
	tasks[index].args[0xD] = showMultichoice(&box, choices);
	tasks[index].function = (u32) profAdviceHandler;
}

void profAdviceHandler(u8 index) {
	switch(multichoice_get_response()) {
	case 0:
	case 1:
	case 2:
		hideMultichoice(tasks[index].args[0xD]);
		tasks[index].function = (u32) profSlideLeft;
		break;
	}
}

void profSlideLeft(u8 index) {
	if (bgid_get_x_offset(2) < 0) {
		bgid_mod_x_offset(2, 0x200, 1);
		return;
	}

	tasks[index].function = (u32) profThisWorld;
}


void pokeball_callback(object *self) {
	if (self->x > 0x48) {
		self->x -= 1;
	}
}

/* How long each running frame will be visible for */
#define FRAME_DURATION 0xA

/* Closed pokeball */
frame ball_closed[] = {
	{ 0x0, FRAME_DURATION },
	{ 0xFFFF, 0 }
};

frame ball_open[] = {
	{ 0x0, FRAME_DURATION },
	{ 0x4, FRAME_DURATION },
	{ 0xFFFF, 0 }
};

rotscale_frame ball_roll_left[] = {
	{ 0, 0, 0x18, 0x10, 0 },
	{ 0, 0, 0x8, 0x10, 0 },
	{ 0x7FFF, 0, 0, 0, 0 }
};

/* Animation tables */
frame *pAnimPokeball[] = { ball_closed, ball_open };
rotscale_frame *pAnimRoll[] = { ball_roll_left };

/* OAM Template */
sprite oam = { 0x000, 0x4000, 0x800, 0x0 };

/* Pokeball */
resource *pokeballGraphics = (resource*) 0x0826056C;
resource *pokeballPalette = (resource*) 0x082605CC;
template pokeballTemplate = { 0xD6D8, 0xD6D8, (sprite*) 0x0826062C, pAnimPokeball, 0, pAnimRoll, pokeball_callback };
//template *pokeballTemplate = (template*) 0x082606F4;

void profThisWorld(u8 index) {
	showMessage(caPokeball);
	tasks[index].function = (u32) pokeballRoll;
}

void pokeballRoll(u8 index) {
	/*
	 * Wait for A button
	 */
	if (check_a_pressed(0)) return;

	/*
	 * Close the box, and load the graphics.
	 */
	textbox_close();
	object_load_compressed_graphics(pokeballGraphics);
	object_load_compressed_palette(pokeballPalette);

	/*
	 * Callback takes care of the roll, all we need to do is display
	 */
	object_display(&pokeballTemplate, 0x60 , 0x75, 1);
	//((void (*)(u8)) 0x08130F2D)(index);


	display_ioreg_set(0x50, 0x2F00);
	display_ioreg_set(0x52, 0x1F00);

	tasks[index].args[6] = 0x20;
	tasks[index].function = (u32) pokeballOpen;
}

/*
 * State for controlling the jumping of the bunny
 */
typedef struct jumpState {
	u8 tracker;
	u8 timer;
	u8 stepper;
	u8 exp;
} jumpState;

jumpState *state = (jumpState*) 0x020370B8;

void pokemon_callback(object *self) {
	switch (state->tracker) {
	case 1:
		if (state->timer) {
			/*
			 * Move the sprite up an increasing amount
			 */
			self->y -= state->exp;
			state->stepper++;

			if (state->stepper == 5) {
				/*
				 * Make the increment larger every five frames
				 * Also move the sprite right
				 */
				self->x += (7 - state->exp);
				state->exp++;
				state->stepper = 0;
			}

			state->timer--;
		} else {
			/*
			 * Reset the state for the next step
			 */
			state->tracker++;
			state->stepper = 0;
			state->exp = 1;
			state->timer = 0x15;
		}
		break;
	case 2:
		if (state->timer) {
			self->y += state->exp;
			state->stepper++;

			if (state->stepper == 5) {
				/*
				 * Make the increment larger every five frames
				 * Also move the sprite right
				 */
				self->x++;
				state->exp++;
				state->stepper = 0;
			}

			state->timer--;
		} else {
			/*
			 * Reset the state for the next step
			 */
			state->tracker++;
			state->stepper = 0;
			state->exp = 1;
			state->timer = 0x15;
		}
	}
}

#define PROFESSOR_POKEMON 0x1
#define POKEMON_FRONT_SPRITE (0x0888CF7C + PROFESSOR_POKEMON * sizeof(resource))
#define POKEMON_PALETTE (0x0888F400 + PROFESSOR_POKEMON * sizeof(resource))

/* Pokeball */
sprite pokeoam = { 0x500, 0xC000, 0x400, 0x0 };
resource *pokemonGraphics = (resource*) POKEMON_FRONT_SPRITE;
resource *pokemonPalette = (resource*) POKEMON_PALETTE;
template pokemonTemplate = { PROFESSOR_POKEMON, PROFESSOR_POKEMON, &pokeoam, (frame **) 0x08231CF0, (u32 *) 0x82346F8, (rotscale_frame **) 0x08234944, pokemon_callback };

void pokeballOpen(u8 index) {
	/*
	 * Wait
	 */

	if (tasks[index].args[6]) {
		tasks[index].args[6] -= 1;
		return;
	}

	state->tracker = 0;
	state->timer = 0;

	/*
	 * Load the Pokemon graphics
	 */
	object_load_compressed_graphics(pokemonGraphics);
	object_load_compressed_palette(pokemonPalette);
	object_display(&pokemonTemplate, objects[0].x, 0x70, 1);

	tasks[index].args[6] = 0x23;

	//tasks[index].function = (u32) 0x0804B9E8 + 1;
	tasks[index].function = (u32) releaseTheBunny;
}

void releaseTheBunny(u8 index) {
	if (tasks[index].args[6] == 0x10) {
		/*
		 * Open the Pokeball
		 */
		objects[0].anim_number = 1;
	}

	if (tasks[index].args[6]) {
		tasks[index].args[6] -= 1;
		return;
	}

	/*
	 * Start the animation
	 */
	state->tracker = 1;
	state->timer = 0x15;
	state->exp = 1;

	/*
	 * Particle spawn. x, y, ?, ?
	 */
	((void (*)(u8,u8,u8,u8)) 0x0804B8C9)(objects[0].x, objects[0].y, 0, 0);

	/*
	 * Glow and flash, ?, ?, ?
	 */
	((void (*)(u8,u8,u8)) 0x0804B8F1)(1, 0, 0xFF);

	display_ioreg_set(0x52, 0x1F);

	//((void (*)(u32,u8)) 0x0800843D)(&objects[1], 1);

	tasks[index].args[6] = 0x20;
	tasks[index].function = (u32) freePokeBall;
}

void freePokeBall(u8 index) {
	if (tasks[index].args[6]) {
		tasks[index].args[6] -= 1;
		return;
	}

	object_delete_and_free(&objects[0]);
	tasks[index].function = (u32) waitForJumpingFinish;
}

void waitForJumpingFinish(u8 index) {
	if (state->tracker < 3) return;

	showMessage(caFirstSpeech);

	tasks[index].args[6] = FADE_STEPS;
	tasks[index].args[7] = 0x0;
	tasks[index].args[8] = 0x10;
	tasks[index].function = (u32) fadeBunny;
}

void fadeBunny(u8 index) {
	u16 bg = tasks[index].args[7];
	u16 fg = tasks[index].args[8];

	if (check_a_pressed(0)) return;

	textbox_close();

	if (bg < 0x12) {
		if (!tasks[index].args[6]) {
			tasks[index].args[7] = bg + 1;
			tasks[index].args[8] = (fg) ? (fg - 1) : 0;
			display_ioreg_set(0x52, (bg << 8) | fg);

			tasks[index].args[6] = FADE_STEPS;
		} else {
			tasks[index].args[6] -= 1;
		}
	} else {
		/*
		 * Free up
		 */
		u8 i;
		for (i = 0; i < 4; ++i)
			object_delete_and_free(&objects[i]);

		tasks[index].function = (u32) explainYourself;
	}
}

void explainYourself(u8 index) {
	showMessage(caTellMe);
	tasks[index].function = (u32) profFadeOutAndFree;
}

void profFadeOutAndFree(u8 index) {
	if (check_a_pressed(0)) return;

	/*
	 * Use built in function to fade professor
	 */
	u16 (*fade)(u8, u8) = (u16 (*)(u8,u8)) 0x0813144C + 1;
	fade(index, 2);

	showMessage(caBoyOrGirl);
	tasks[index].function = (u32) areYouABoyOrGirl;
}

void areYouABoyOrGirl(u8 index) {
	if (check_a_pressed(0)) return;

	/*
	 * Hide professor background layer
	 */
	gpu_sync_bg_hide(2);

	tasks[index].function = (u32) chooseGender;
}

