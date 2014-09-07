#include "oakintro.h"
#include "strings.h"
#include "images/lucas.h"
#include "images/tilemap.h"
#include "images/background.h"
#include "images/tileset.h"
#include "engine/video.h"
#include "engine/callback.h"
#include "engine/script.h"

extern void chooseGender(u8 index);
void callback (u8 index);

void helloThere(u8 index);
void showRowan(u8 index);
void introduceRowan(u8 index);
void preRowanMultichoice(u8 index);
void slideRowanMultichoice(u8 index);
void handleRowanMultichoice(u8 index);
void thisWorld(u8 index);
void releaseBuneary(u8 index);
void preChooseGender(u8 index);
void unfadeBoy(u8 index);
void unfadeGirl(u8 index);
void boyGirl(u8 index);
void returnFromRivalName(u8 index);

u16 (*lcd_io_set)(u8, u16) = (u16 (*)(void)) 0x08000A38 + 1; 
//
//void showMessage(char *message) {
//	char *dest = (char*) 0x02021D18;
//	u8 b = 8;
//
//	loadMessageBox(0, 0);
//	fdecoder(dest, message);
//	box_related_one(0, 4, dest, b, 0, 2, 1, 3);
//
//	rboxid_to_vram(0, 3);
//}

void fadeScreen() {
        fadescreen(0xFFFFFFFF, 0x0, 0, 0x10, 0x0000);
}

void unfadeScreenWhite() {
        fadescreen(0xFFFFFFFF, 0x0, 0x10, 0, 0x7FFF);
}

void fadeScreenWhite() {
        fadescreen(0xFFFFFFFF, 0x0, 0, 0x10, 0x7FFF);
}
void unfadeScreen();

#define REG_DISPCNT         *(u16*)0x4000000

void returnFromPlayerName(u8 index);

void callback (u8 index) {
	u16 arg;

	arg = tasks[index].args[6];

	if (!arg) {
		u32 size;
		u8 i;
		u8 *ptr;
		void *data;

		data = malloc_and_LZ77UnComp((void *) backgroundTiles, &size);

		gpu_copy_to_tileset(1, data, size , 0);
		gpu_copy_tilemap(1, backgroundMap, 0, 0);
		gpu_pal_apply(backgroundPal, 0x00, 0x40);

		bgid_send_tilemap(1);

		free(data);

		/* Partial unfade. Don't cover message box */
		//fadescreen(0xFFFFFFF0, 0x0, 0x10, 0x0, 0x0000);

		/* How long to wait - in frames? */
		tasks[index].args[6] = 0x30;

		//fadescreen(0xFFFFFFFF, 0x0, 0x10, 0x0, 0x0000);
		fadescreen(0xFFFFFFFF, 0x10, 0x10, 0x0, 0x0000);

		switch(variables[0xC]) {
		case 0:
			tasks[index].function = (u32) showRowan;
			break;
		case 1:
			tasks[index].function = (u32) returnFromPlayerName;
			break;
		case 2:
			tasks[index].function = (u32) returnFromRivalName;
			break;
		}
	} else {
		tasks[index].args[6] -= 1;
	}
}

// { 0xFFFE, 0 } means loop
// { 0xFFFF, 0 } means end
// You can toggle between animations by setting anim_number

//frame anim[] = { { 0x0, 0x10 }, { 0x100, 0x10  }, { 0x80, 0x10  }, { 0x180, 0x10  }, { 0xFFFE, 0 } };
//frame anim2[] = { { 0x40, 0x10 }, { 0x140, 0x10  }, { 0xC0, 0x10  }, { 0x1C0, 0x10  }, { 0xFFFE, 0 } };
//frame **pAnimTop = &anim;
//frame **pAnimBottom = &anim2;

void object_cb (object *self) {

}

//sprite oam = { 0x400, 0xC000, 0x800, 0x0 };
//sprite oam2 = { 0x2400, 0xC000, 0x800, 0x0 };
////resource graphics = { 0x0846163C, 0x7FFF, 0x1000 };
//resource graphics = { lucasTiles, 0x7FFF, 0x1000 };
////resource palette = { 0x84629D0, 0x1000, 0x1000  };
//resource palette = { lucasPal, 0x1000, 0x1000  };
//template temp = { 0x1000, 0x1000, &oam, &pAnimTop, 0, 0x8231CFC, 0x800760D };
////template temp2 = { 0x1000, 0x1000, &oam, &ptra, 0, 0x8231CFC, 0x800760D };
//template temp2 = { 0x1000, 0x1000, &oam, &pAnimBottom, 0, 0x8231CFC, &object_cb };

void helloThere(u8 index) {
	u16 arg;

	arg = tasks[index].args[6];
	if (!arg) {
		u32 **ptr = (u32**) 0x0203B108;
		char *dest = (char*) 0x02021D18;
		u8 b = 8;
		u8 i;

		u16 (*object_from_compressed)(u32*) = (u16 (*)(void)) 0x0800EBCC + 1;
		u8 (*object_apply_palette)(u32*) = (u8 (*)(void)) 0x08008928 + 1;
		u8 (*object_search)(u32*,u16,u16,u8) = (u8 (*)(void)) 0x08006F8C + 1;
		void (*load_pal)(u32*,u16,u16) = (void (*)(void)) 0x080703EC + 1;
		void (*lcd_io_set)(u8,u16) = (void (*)(void)) 0x08000A38 + 1;

		//object_from_compressed((u32*) 0x8462F14);
		//object_apply_palette((u32*) 0x8462F24);

		//object_from_compressed((u32*) &graphics);
		//object_apply_palette((u32*) &palette);

		// For 256
		//load_pal((u32*) 0x084615FC, 0x100 + 4 * 16, 16 * 2 * 2);

		// Set BLDCNT to allow alpha blending of OAM onto bg
		//lcd_io_set(0x50, 0x2F50);

		// Set opacity of the sprites
		//lcd_io_set(0x52, 0x40C);

		// Now we can toggle the one objects opacity bit to set which objects are blended

		// This isn't a search, it controls display of objects
		//object_search((u32*) &temp, 64, 0x30, 1);
		// object_search((u32*) &temp2, 64, 0x70, 1);

		loadMessageBox(0, 0);
		fdecoder(dest, (char*) caHelloThere);
		box_related_one(0, 4, dest, b, 0, 2, 1, 3);

		rboxid_to_vram(0, 3);
	
		tasks[index].args[6] = 0x1E;
		tasks[index].function = (u32) showRowan;
	} else {
		tasks[index].args[6] = arg - 1;
	}
	return;
}

void showRowan(u8 index) {
	//task *tasks = (task *) 0x3005090;

	/* Wait for A press */
	if (check_a_pressed(0)) return;

	u16 arg = tasks[index].args[6];
	if (!arg) {
		textbox_close();
		unfadeScreen();
		song_play_for_text(0x124);

		/* Show person. There's gonna be a memory leak */
		//bgid_send_tilemap(2);

		tasks[index].args[6] = 0xA0;
		tasks[index].function = (u32) chooseGender;//introduceRowan;
	} else {
		tasks[index].args[6] = arg - 1;
	}
	return;
}

void introduceRowan(u8 index) {
	//task *tasks = (task *) 0x3005090;

	u16 arg;
	u8 mix_color;

	mix_color = (u8) fade_control->color;
	if (mix_color & 0x80) return;

	arg = tasks[index].args[6];
	if (!arg) {
		u32 **ptr = (u32**) 0x0203B108;
		char *dest = (char*) 0x02021D18;
		u8 b = 8;

		loadMessageBox(0, 0);
		fdecoder(dest, (char*) caIntroduce);
		box_related_one(0, 4, dest, b, 0, 2, 1, 3);

		rboxid_to_vram(0, 3);
	
		tasks[index].args[6] = 0x0;
		tasks[index].function = (u32) preRowanMultichoice;
	} else {
		tasks[index].args[6] = arg - 1;
	}
	return;
}

void multichoice() {
	u16 id;
	u8 current;

	/* Wait for A press */
	if (check_a_pressed(0)) return;

	/* Show multichoice */

	// Load rbox
	u16 (*func)(u32) = (u16 (*)(void)) 0x08003CE4 + 1;
	//id = func(&boy_girl);

	// New
	//tasks[index].args[0xD] = id;
	u16 (*func2)(u16) = (u16 (*)(void)) 0x8003FA0 + 1;
	func2(id);

	// Draw its border
	int (*outline)(u16, u8, u16, u8) = (int (*)(void)) 0x0810F2E8 + 1;
	outline(id, 1, 0x214, 0xE);

	// Clear the contents
	int (*clear)(u16, u8) = (int (*)(void)) 0x0800445C + 1;
	clear(id, 0x11);

	// Draw strings
	// id, font, x, y, bg_color, fg_color, str
	int (*print)(u16, u8, u8, u8, u32, u32, char*) = (int (*)(void)) 0x0812E51C + 1;

	print(id, 2, 8, 1, 1, 0, caOptionInfoControls);
	print(id, 2, 8, 0x11, 1, 0, caOptionInfoAdventure);
	//print(id, 2, 8, 0x21, 1, 0, 0x08415D97);

	// Field
	u8 (*field)(u8, u8) = (u8 (*)(void)) 0x080F79D8 + 1;
	current = field(2, 1);

	// Allow moving the selecty thing
	int (*huh)(u8, u8, u8, u8, u8, u8, u32) = (int (*)(void)) 0x0810F7D8 + 1;
	huh(id, 2, 0, 1, current + 2, 2, 0);

	rboxid_to_vram(id, 3);
}

void preRowanMultichoice(u8 index) {
	//task *tasks = (task *) 0x3005090;
	char *dest = (char*) 0x02021D18;
	u32 *okay = (u32*) 0x2020050;

	u8 *prev_quest_mode = (u8*) 0x0203ADFA;
	u8 b = 8;

	/* Wait for A press */
	if (check_a_pressed(0)) return;
	//*textflags = 0xF;
	//*prev_quest_mode = 2;

	// Textflags = 4 (3rd bit) means no continue arrow, bit 2 means auto a press
	// We use auto A press to wait for message to finish rendering
	// By checking for a press
	
	loadMessageBox(0, 0);
	fdecoder(dest, (char*) caAdvice);
	box_related_one(0, 4, dest, b, 0, 2, 1, 3);

	rboxid_to_vram(0, 3);

	tasks[index].args[6] = 0x1E;

	tasks[index].function = (u32) slideRowanMultichoice;
}

void slideRowanMultichoice(u8 index) {
	//task *tasks = (task *) 0x3005090;
	u32 **ptr = (u32**) 0x0203B108;
	u16 *		displace = (u16*) 0x02021BC8;
	u16 arg;
	u16 id;
	u8 current;

	/* Wait message */
	if (check_a_pressed(0)) return;

	arg = tasks[index].args[6];

	if (arg > 0) {
		/* Slide sprite right */
		u16 (*mod)(u8,u32,u8) = (u16 (*)(void)) 0x08001B90 + 1;
		mod(2, 0x200, 2);
		tasks[index].args[6] = arg - 1;
		return;
	}

	/* Show multichoice */

	// Load rbox
	u16 (*func)(u32) = (u16 (*)(void)) 0x08003CE4 + 1;
	//id = func(&info_multichoice);

	// New
	tasks[index].args[0xD] = id;
	u16 (*func2)(u16) = (u16 (*)(void)) 0x8003FA0 + 1;
	func2(id);

	// Draw its border
	int (*outline)(u16, u8, u16, u8) = (int (*)(void)) 0x0810F2E8 + 1;
	outline(id, 1, 0x214, 0xE);

	// Clear the contents
	int (*clear)(u16, u8) = (int (*)(void)) 0x0800445C + 1;
	clear(id, 0x11);

	// Draw strings
	// id, font, x, y, bg_color, fg_color, str
	int (*print)(u16, u8, u8, u8, u32, u32, u32*) = (int (*)(void)) 0x0812E51C + 1;

	print(id, 2, 8, 1, 1, 0, caOptionInfoControls);
	print(id, 2, 8, 0x11, 1, 0, caOptionInfoAdventure);
	print(id, 2, 8, 0x21, 1, 0, caOptionInfoNone);

	// Field
	u8 (*field)(u8, u8) = (u8 (*)(void)) 0x080F79D8 + 1;
	current = field(2, 1);

	// Allow moving the selecty thing
	int (*huh)(u8, u8, u8, u8, u8, u8, u32) = (int (*)(void)) 0x0810F7D8 + 1;
	huh(id, 2, 0, 1, current + 2, 3, 0);

	rboxid_to_vram(id, 3);
	
	tasks[index].function = (u32) handleRowanMultichoice;
}

void handleRowanMultichoice(u8 index) {
	u16 id;
	//task *tasks = (task *) 0x3005090;
	int (*freebox)(u8) = (int (*)(void)) 0x0810F4D8 + 1;
	int (*clearbox)(u16) = (int (*)(void)) 0x08003E3C + 1;
	int (*finalbox)(u8, u8) = (int (*)(void)) 0x80F6F54 + 1;
	u8 (*multichoiceGetResponse)() = (u8 (*)(void)) 0x0810FA04 + 1;

	id =  tasks[index].args[0xD];

	switch(multichoiceGetResponse()) {
	case 0:
		//tasks[index].function = (u32) 0x0812EF50 + 1;
		//break;
	case 1:
		//tasks[index].function = (u32) 0x0812F4A8 + 1;
		//break;
	case 2:
		// All three are needed to clear the multichoice. Also clears textbox
		freebox(1);
		clearbox(id);
		finalbox(0, 1);

		tasks[index].args[6] = 0x1E;
		tasks[index].function = (u32) thisWorld;
		break;
	}
}

void thisWorld(u8 index) {
	//task *tasks = (task *) 0x3005090;
	u32 **ptr = (u32**) 0x0203B108;
	u16 *		displace = (u16*) 0x02021BC8;
	u16 arg;
	u16 id;
	u8 current;
	u32 *textflags = (u32*) 0x03003E50;

	//*textflags = 0x0;

	// Transparency fade
	//u16 (*fade)(u8, u8) = (u16 (*)(void)) 0x0813144C + 1;

	arg = tasks[index].args[6];
	
	if (arg > 0) {
		/* Slide sprite left */
		u16 (*mod)(u8,u32,u8) = (u16 (*)(void)) 0x08001B90 + 1;
		mod(2, 0x200, 1);
		tasks[index].args[6] = arg - 1;
		return;
	}
	
	showMessage(caThisWorld);
	tasks[index].args[6] = 0x1E;
	tasks[index].function = (u32) releaseBuneary;
	//fade(index, 2);
}

void releaseBuneary(u8 index) {
	//task *tasks = (task *) 0x3005090;

	if (check_a_pressed(0)) return;

	tasks[index].args[6] = 0x1E;
	tasks[index].function = (u32) preChooseGender;
}

void preChooseGender(u8 index) {
	//task *tasks = (task *) 0x3005090;

	//if (check_a_pressed(0)) return;
	
	u16 (*fade)(u8, u8) = (u16 (*)(void)) 0x0813144C + 1;
	fade(index, 2);

	tasks[index].args[6] = 0x70;
	//tasks[index].function = (u32) chooseGender;
}

void loadBigSprite2(u32 img, u32 pal, u32 dest, u8 bgid, u8 x) {
	u8 *ptr;
	void *data;
	u8 i;

	gpu_pal_apply(pal, 0x60, 0x40);
	LZ77UnCompVram(img, dest);
	data = malloc_and_clear(0x60);
	/* Generate tilemap on the fly */
	ptr = (u8*) data;
	for (i = 0; i < 0x60; ++i) {
		ptr[i] = i;
	}
	int (*func)(u8,u8*,u8,u8,u8,u8,u8,u8,u8,u8,u8,u8) = (int (*)(void)) 0x0800226C + 1;
	/* BGID, tilemap, ?, ?, width, height ,x, y, width, height, ?, offset */
	func(bgid, ptr, 0, 0, 8, 0xC, x, 2, 8, 0xC, 0x10, 0x18);
	gpu_sync_bg_show(bgid);
	bgid_send_tilemap(bgid);
}

void chooseGender2(u8 index) {
	task *tasks = (task *) 0x3005090;

	if (tasks[index].args[6]) {
		tasks[index].args[6] -= 1;
		return;
	}

	//loadBigSprite(0x0846163C, 0x084615FC, 0x6000C00, 3, 0x0);
	//loadBigSprite(0x08461D14, 0x08461CD4, 0x6000600, 2, 0xB);

	tasks[index].args[6] = 0x1E;
	tasks[index].function = (u32) unfadeBoy;
}

void unfadeBoy(u8 index) {
	task *tasks = (task *) 0x3005090;

	if (tasks[index].args[6]) {
		tasks[index].args[6] -= 1;
		return;
	}
	
	u16 (*unfade)(u8, u8) = (u16 (*)(void)) 0x81315CC + 1;
	unfade(index, 2);
	unfade(index, 3);

	tasks[index].args[6] = 0x1E;
	tasks[index].function = (u32) unfadeGirl;
}

void unfadeGirl(u8 index) {

}

void boyGirl(u8 index) {
	
}

void unfadeScreen() {
        fadescreen(0xFFFFFFFF, 0x10, 0x10, 0x0, 0x0000);
}

void wait() {

}

