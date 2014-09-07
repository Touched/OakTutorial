#include "oakintro.h"
#include "engine/video.h"
#include "engine/callback.h"
#include "engine/input.h"
#include "engine/audio.h"
#include "engine/text.h"
#include "global.h"
#include "engine/player.h"
#include "images/lucas.h"
#include "images/dawn.h"
#include "strings.h"

/* What BLDALPHA will be set to when half faded */
#define PARTIAL_FADE 0xA03

/* How long each running frame will be visible for */
#define FRAME_DURATION 0xA

/* Individual animations. Pair for each half */
frame anim[] = { { 0x0, FRAME_DURATION }, { 0x100, FRAME_DURATION }, { 0x80, FRAME_DURATION }, { 0x180, FRAME_DURATION }, { 0xFFFE, 0 } };
frame anim2[] = { { 0x40, FRAME_DURATION }, { 0x140, FRAME_DURATION }, { 0xC0, FRAME_DURATION }, { 0x1C0, FRAME_DURATION }, { 0xFFFE, 0 } };
frame still[] = { { 0x0, FRAME_DURATION } , { 0xFFFF, 0x0 } };
frame still2[] = { { 0x40, FRAME_DURATION } , { 0xFFFF, 0x0 } };

/* Animation table */
frame *pAnimTop[] = { anim, still };
frame *pAnimBottom[] = { anim2, still2 };

/* OAM Template */
sprite oam = { 0x400, 0xC000, 0x800, 0x0 };

typedef struct genderState {
	u32 dawnAnimate : 1;
	u32 dawnOpaque : 1;
	u32 dawnCenter : 1;
	u32 dawnInvisible : 1;
	u32 lucasAnimate : 1;
	u32 lucasOpaque : 1;
	u32 lucasCenter : 1;
	u32 lucasInvisible : 1;
	u32 choice : 1;
	u8 lucasFrame;
	u8 dawnFrame;
} genderState;

genderState *state = (genderState*) 0x020370B8;
//genderState *state = (genderState*) 0x0203B108;

extern void namePlayer(u8 index);

void lucas_callback(object *self) {
	/* Check if lucas must be semi-transparent */

	if (state->lucasOpaque) {
		self->final_oam.attr0 &= 0xFBFF;
	} else {
		self->final_oam.attr0 = (self->final_oam.attr0 & 0xFBFF) | 0x400;
	}

	if (state->lucasAnimate) {
		/* While we're animating, store the current frame */
		state->lucasFrame = self->anim_frame;
	} else {
		/* Pause on the last known frame */
		self->anim_frame = state->lucasFrame;
	}

	/*
	 * Toggle visiblity
	 */
	self->bitfield2 = (self->bitfield2 & 0xFB) | (state->lucasInvisible ? 4 : 0);

	if (state->lucasCenter) {
		/* We must centre Lucas */
		if (self->x > 0x77) {
			self->x -= SLIDE_STEP;
		} else {
			/* He's centered; clear the bit */
			state->lucasCenter = 0;
		}
	}
}

void dawn_callback(object *self) {
	/* Check if Dawn must be semi-transparent */

	if (state->dawnOpaque) {
		self->final_oam.attr0 &= 0xFBFF;
	} else {
		self->final_oam.attr0 = (self->final_oam.attr0 & 0xFBFF) | 0x400;
	}

	if (state->dawnAnimate) {
		state->dawnFrame = self->anim_frame;
	} else {
		self->anim_frame = state->dawnFrame;
	}

	/*
	 * Toggle visiblity
	 */
	self->bitfield2 = (self->bitfield2 & 0xFB) | (state->dawnInvisible ? 4 : 0);

	if (state->dawnCenter) {
			/* We must centre Lucas */
			if (self->x < 0x77) {
				self->x += SLIDE_STEP;
			} else {
				/* He's centered; clear the bit */
				state->dawnCenter = 0;
			}
	}
}

/* Lucas */
resource lucasGraphics = { lucasTiles, 0x4000, 0x1000 };
resource lucasPalette = { lucasPal, 0x1000, 0x1000  };
template lucasTop = { 0x1000, 0x1000, &oam, &pAnimTop, 0, 0x8231CFC, lucas_callback };
template lucasBottom = { 0x1000, 0x1000, &oam, &pAnimBottom, 0, 0x8231CFC, lucas_callback };

/* Dawn */
resource dawnGraphics = { dawnTiles, 0x4000, 0x200 };
resource dawnPalette = { dawnPal, 0x200, 0x200  };
template dawnTop = { 0x200, 0x200, &oam, &pAnimTop, 0, 0x8231CFC, dawn_callback };
template dawnBottom = { 0x200, 0x200, &oam, &pAnimBottom, 0, 0x8231CFC, dawn_callback };

u16 loadBigSprite(resource *graphics, resource *palette, template *top, template *bottom, u16 x, u16 y) {
	u16 out;

	object_load_compressed_graphics((u32*) graphics);
	object_load_palette((u32*) palette);

	// Set BLDCNT to allow alpha blending of OAM onto bg
	//display_ioreg_set(0x50, 0x2F50);

	// Set opacity of the sprites
	//display_ioreg_set(0x52, 0x40C);

	// Now we can toggle the one objects opacity bit to set which objects are blended

	out = object_display((u32*) top, x, y, 1) >> 8;
	out |= object_display((u32*) bottom, x, y + 0x40, 1);

	// Should store object ID somewhere, we may need it for later
	return out;
}

void blank(u8 i) {

}

void fadeInLucas(u8 index);
void fadeInDawn(u8 index);
void genderChooser(u8 index);
void confirmGenderChoice(u8 index);
void confirmGenderChoiceHandler(u8 index);
void slideDawn(u8 index);
void slideLucas(u8 index);
void repeatGender(u8 index);
void setGender(u8 index);
void repeatGenderFree(u8 index);
void preFadeDawn(u8 index);
void preRepeatGender(u8 index);

void chooseGender(u8 index) {
	//loadBigSprite(&dawnGraphics, &dawnPalette, &dawnTop, &dawnBottom, 0x30, 0x20);
	u16 ids = loadBigSprite(&lucasGraphics, &lucasPalette, &lucasTop, &lucasBottom, 0xC0, 0x20);

	/* We use the semi-transparent OAM mode to select 1st target */
	display_ioreg_set(0x50, 0x2F00);
	display_ioreg_set(0x52, 0x1F00);

	tasks[index].args[6] = FADE_STEPS;
	tasks[index].args[7] = 0x10;
	tasks[index].args[8] = 0x0;
	tasks[index].function = (u32) fadeInLucas;

	/*
	 * Initialise the state
	 */
	state->choice = 0;
}

void fadeInLucas(u8 index) {
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
		/* Set Lucas to be opaque */
		state->lucasOpaque = 1;

		/*
		 * This extra step is needed to avoid a momentary flash on Lucas's
		 * part, caused by the bldcnt being reset before he becomes opaque.
		 * The extra frame's wait will allow lucas's callback to be called once
		 * and his object attributes will be correctly set.
		 */
		tasks[index].function = (u32) preFadeDawn;
	}
}

void preFadeDawn(u8 index) {
	/* Fade out again */
	display_ioreg_set(0x52, 0x1F00);
	tasks[index].args[6] = FADE_STEPS;
	tasks[index].args[7] = 0x10;
	tasks[index].args[8] = 0x0;

	loadBigSprite(&dawnGraphics, &dawnPalette, &dawnTop, &dawnBottom, 0x30, 0x20);
	tasks[index].function = (u32) fadeInDawn;
}

void fadeInDawn(u8 index) {
	u16 bg = tasks[index].args[7];
	u16 fg = tasks[index].args[8];

	if (bg) {
		if (!tasks[index].args[6]) {
			tasks[index].args[7] = bg - 1;
			tasks[index].args[8] = fg + 1;
			display_ioreg_set(0x52, (bg << 8) | fg);

			tasks[index].args[6] = FADE_STEPS;
		} else {
			tasks[index].args[6] -= 1;
		}
	} else {

		tasks[index].args[6] = 0x1E;
		tasks[index].function = (u32) genderChooser;
	}
}

void genderChooser(u8 index) {
	/*
	 * Small delay here makes it look better.
	 */

	if (tasks[index].args[6] > 0 ) {
		tasks[index].args[6] -= 1;
	}

	if (tasks[index].args[6] == 1) {
		/* Do a half fade */
		display_ioreg_set(0x52, PARTIAL_FADE);
	}

	/*
	 * This task will run until the user presses 'A'.
	 * It will check for button presses and set the appropriate state struct bits.
	 */
	if (buttons->left || buttons->right) {
		/* Bundle into a single case, we only have two options anyway */
		state->choice = !state->choice;
		audio_play(SOUND_CLINK);
	}

	if (state->choice) {
		/* Girl picked */
		state->lucasAnimate = 0;
		state->lucasOpaque = 0;
		state->dawnAnimate = 1;
		state->dawnOpaque = 1;
	} else {
		/* Boy picked */
		state->lucasAnimate = 1;
		state->lucasOpaque = 1;
		state->dawnAnimate = 0;
		state->dawnOpaque = 0;
	}

	if (buttons->A) {
		/* Player has accepted the choice, confirm their decision */
		tasks[index].args[6] = FADE_STEPS;
		tasks[index].args[7] = 0xA;
		tasks[index].args[8] = 0x3;

		if (state->choice) {
			/* Request the callback to centre Dawn */
			state->dawnCenter = 1;
			state->dawnAnimate = 0;

			tasks[index].function = (u32) slideDawn;
		} else {
			/* Request the callback to centre Dawn */
			state->lucasCenter = 1;
			state->lucasAnimate = 0;
			tasks[index].function = (u32) slideLucas;
		}
	}
}

void slideLucas(u8 index) {
	/* Fade out Dawn */
	u16 bg = tasks[index].args[7];
	u16 fg = tasks[index].args[8];

	if (bg < 0x12) {
		if (!tasks[index].args[6]) {
			tasks[index].args[7] = bg + 1;
			tasks[index].args[8] = (fg) ? (fg - 1) : 0;
			display_ioreg_set(0x52, (bg << 8) | fg);

			tasks[index].args[6] = FADE_STEPS;
		} else {
			tasks[index].args[6] -= 1;
		}
	}

	if (state->lucasCenter) {
		/* Lucas is still being centred */
		return;
	}

	/* Dawn is still being faded */
	if (bg < 0x12) return;

	showMessage(caConfirmBoy);
	tasks[index].function = (u32) confirmGenderChoice;
}

void slideDawn(u8 index) {
	/* Fade out Lucas */
	u16 bg = tasks[index].args[7];
	u16 fg = tasks[index].args[8];

	if (bg < 0x12) {
		if (!tasks[index].args[6]) {
			tasks[index].args[7] = bg + 1;
			tasks[index].args[8] = (fg) ? (fg - 1) : 0;
			display_ioreg_set(0x52, (bg << 8) | fg);

			tasks[index].args[6] = FADE_STEPS;
		} else {
			tasks[index].args[6] -= 1;
		}
	}

	if (state->dawnCenter) {
		/* Dawn is still being centred. Wait for callback to clear bit */
		return;
	}

	/* Lucas is still being faded */
	if (bg < 0x12) return;

	showMessage(caConfirmGirl);
	tasks[index].function = (u32) confirmGenderChoice;
}

void confirmGenderChoice(u8 index) {
	char *choices[] = { caYes, caNo, 0 };

	rbox box = {0, 2, 2, 4, 4, 0xF, 0x130};
	tasks[index].args[0xD] = showMultichoice(&box, choices);

	tasks[index].function = (u32) confirmGenderChoiceHandler;
}

void confirmGenderChoiceHandler(u8 index) {
	u16 id = tasks[index].args[0xD];

	switch(multichoice_get_response()) {
	case 0:
		/* Yes */
		tasks[index].function = (u32) setGender;
		hideMultichoice(id);
		break;
	case 1:
		/* No */
		tasks[index].args[6] = FADE_STEPS;
		tasks[index].args[7] = 0x0;
		tasks[index].args[8] = 0x10;

		/*
		 * Allow fading out
		 */

		state->dawnOpaque = 0;
		state->lucasOpaque = 0;

		/*
		 * Set the unchosen OAM to be invisible
		 */

		if (state->choice) state->lucasInvisible = 1;
		else state->dawnInvisible = 1;

		tasks[index].function = (u32) preRepeatGender;

		hideMultichoice(id);
		break;
	}
}

void preRepeatGender(u8 index) {
	/*
	 * Reset the bldcnt to begin fadeout. We need this extra step here,
	 * otherwise, Dawn/Lucas will momentarily flash after selecting "No".
	 * Again, this gives time fall the callbacks to do their business and
	 * properly hide the unchosen character.
	 */
	display_ioreg_set(0x52, 0x1F);

	tasks[index].function = (u32) repeatGender;
}

void repeatGender(u8 index) {
	/*
	 * Fade out
	 */

	u16 bg = tasks[index].args[7];
	u16 fg = tasks[index].args[8];

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
		display_ioreg_set(0x52, 0x1F00);

		tasks[index].args[6] = 0x1E;
		tasks[index].function = (u32) repeatGenderFree;
	}
}

void repeatGenderFree(u8 index) {
	/*
	 * Destroy the objects and try again.
	 */

	int i =0;

	if (tasks[index].args[6]) {
		tasks[index].args[6] -= 1;
		return;
	}

	state->dawnInvisible = 0;
	state->lucasInvisible = 0;

	void (*object_free_all)() = (void (*)(void)) 0x08007770 + 1;
	void (*gpu_pal_free_by_tag)(u16) = (void (*)(void)) 0x08008A30 + 1;
	void (*gpu_tile_obj_free_by_tag)(u32*) = (void (*)(void)) 0x0800874C + 1;

//	object_free_all();
	for (i = 0; i < 4; ++i)
		((void (*)(u32*)) 0x08007805)(&objects[i]);

	tasks[index].function = (u32) chooseGender;
}

void setGender(u8 index) {
	/*
	 * Player confirmed their gender, write the choice.
	 */

	(*player)->gender = state->choice;

	tasks[index].function = (u32) namePlayer;
}
