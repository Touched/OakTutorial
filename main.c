#include "oakintro.h"
#include "strings.h"

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
void chooseGender(u8 index);
void unfadeBoy(u8 index);
void unfadeGirl(u8 index);
void boyGirl(u8 index);

u16 (*lcd_io_set)(u8, u16) = (u16 (*)(void)) 0x08000A38 + 1; 

typedef struct task {
	u32 function;
	u8 in_use;
	u8 prev;
	u8 next;
	u8 priority;
	u16 args[16];
} task;

typedef struct fade_ctrl {
	u16 bg_pal;
	u16 obj_pal;
	u16 scale;
	u16 color;
	u8 field8;
	u8 field9;
	u8 fieldA;
	u8 fieldB;
} fade_ctrl;

typedef struct rbox {
	u8 bgid;
	u8 x;
	u8 y;
	u8 w;
	u8 h;
	u8 pal;
	u16 offset;
} rbox;

void showMessage(char *message) {
	char *dest = (char*) 0x02021D18;
	u8 b = 8;

	loadMessageBox(0, 0);
	fdecoder(dest, message);
	box_related_one(0, 4, dest, b, 0, 2, 1, 3);

	rboxid_to_vram(0, 3);
}

rbox boy_girl = {0, 0x12, 9, 9, 4, 0xF, 0x174};

rbox info_multichoice = {0, 2, 2, 15, 6, 0xF, 0x130};

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

void callback (u8 index) {
	u16 arg;
	task *tasks = (task *) 0x3005090;

	//0812EC98
	//u16 (*bg_vram_setup)(u8, u32, u8) = (u16 (*)(void)) 0x08001658 + 1;
	//bg_vram_setup(1, 0x8462E58, 3);

	arg = tasks[index].args[6];

	if (!arg) {
		u32 size;
		u8 i;
		u8 *ptr;

		void *data = malloc_and_LZ77UnComp((void *) 0x08460CA4, &size);

		gpu_copy_to_tileset(1, data, size, 0);
		gpu_copy_tilemap(1, (void *) 0x08460CE8, 0, 0);

		bgid_send_tilemap(1);
		
		/* Load oak */

		gpu_pal_apply(0x084623AC, 0x60, 0x40);
		LZ77UnCompVram(0x84623EC, 0x6000600);
		data = malloc_and_clear(0x60);

		/* Generate tilemap on the fly */
		ptr = (u8*) data;
		for (i = 0; i < 0x60; ++i) {
			ptr[i] = i;
		}

		//bgid_fill_rectangle(2, 0, 0, 0, 0x20, 0x20, 0x10);

		int (*func)(u8,u32*,u8,u8,u8,u8,u8,u8,u8,u8,u8,u8) = (int (*)(void)) 0x0800226C + 1;

		/* BGID, tilemap, ?, ?, width, height ,x, y, width, height, ?, offset */
		func(2, ptr, 0, 0, 8, 0xC, 0xB, 2, 8, 0xC, 0x10, 0x18);

		/* Partial unfade. Don't cover message box */
		fadescreen(0xFFFFFFF0, 0x0, 0x10, 0x0, 0x0000);
		song_play_for_text(0x0);

		gpu_sync_bg_show(2);

		/* How long to wait - in frames? */
		tasks[index].args[6] = 0x30;


		// free(data);

		/* Next task */
		tasks[index].function = (u32) helloThere;
	} else {
		tasks[index].args[6] -= 1;
	}
}

void helloThere(u8 index) {
	task *tasks = (task *) 0x3005090;

	u16 arg;

	arg = tasks[index].args[6];
	if (!arg) {
		u32 **ptr = (u32**) 0x0203B108;
		char *dest = (char*) 0x02021D18;
		u8 b = 8;

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
	task *tasks = (task *) 0x3005090;

	/* Wait for A press */
	if (check_a_pressed(0)) return;

	u16 arg = tasks[index].args[6];
	if (!arg) {
		textbox_close();
		unfadeScreen();
		song_play_for_text(0x124);

		/* Show person. There's gonna be a memory leak */
		bgid_send_tilemap(2);

		tasks[index].args[6] = 0xA0;
		tasks[index].function = (u32) introduceRowan;
	} else {
		tasks[index].args[6] = arg - 1;
	}
	return;
}

void introduceRowan(u8 index) {
	task *tasks = (task *) 0x3005090;

	fade_ctrl *fade_control = (fade_ctrl *) 0x02037AB8;
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
	id = func(&boy_girl);

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
	task *tasks = (task *) 0x3005090;
	char *dest = (char*) 0x02021D18;
	u32 *okay = (u32*) 0x2020050;

	u32 *textflags = (u32*) 0x03003E50;
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
	task *tasks = (task *) 0x3005090;
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
	id = func(&info_multichoice);

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
	task *tasks = (task *) 0x3005090;
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
	task *tasks = (task *) 0x3005090;
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
	task *tasks = (task *) 0x3005090;

	if (check_a_pressed(0)) return;

	tasks[index].args[6] = 0x1E;
	tasks[index].function = (u32) preChooseGender;
}

void preChooseGender(u8 index) {
	task *tasks = (task *) 0x3005090;

	//if (check_a_pressed(0)) return;
	
	u16 (*fade)(u8, u8) = (u16 (*)(void)) 0x0813144C + 1;
	fade(index, 2);

	tasks[index].args[6] = 0x70;
	tasks[index].function = (u32) chooseGender;
}

void loadBigSprite(u32 img, u32 pal, u32 dest, u8 bgid, u8 x) {
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

void chooseGender(u8 index) {
	task *tasks = (task *) 0x3005090;

	if (tasks[index].args[6]) {
		tasks[index].args[6] -= 1;
		return;
	}

	loadBigSprite(0x0846163C, 0x084615FC, 0x6000C00, 3, 0x0);
	loadBigSprite(0x08461D14, 0x08461CD4, 0x6000600, 2, 0xB);

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

