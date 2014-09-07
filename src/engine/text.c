#include "text.h"
char caYes[] = { 0xd3, 0xbf, 0xcd, 0xff };
char caNo[] = { 0xc8, 0xc9, 0xff };

void showMessage(char *message) {
	char *dest = (char*) 0x02021D18;
	u8 speed = 1; /* 0: instant, 8: slow (default) */

	loadMessageBox(0, 0);
	fdecoder(dest, message);
	box_related_one(0, 4, dest, speed, 0, 2, 1, 3);

	rboxid_to_vram(0, 3);
}

rbox boy_girl = {0, 0x12, 9, 9, 4, 0xF, 0x174};

rbox info_multichoice = {0, 2, 2, 15, 6, 0xF, 0x130};

u8 multichoice_field(u8, u8);
int multichoice_prepare(u8 id, u8, u8, u8, u8, u8, u32);
int print_string(u16 id, u8 font, u8 x, u8 y, u32 bg_color, u32 fg_color, char* str);

u16 showMultichoice(rbox *box, char **choices) {
	u16 id;
	u8 current;
	u8 number = 0;

	id = load_rbox(box);

	// New
	fix_box(id);
	outline_box(id, 1, 0x214, 0xE);
	clear_box(id, 0x11);

	// Draw strings
	for (number = 0; choices[number]; ++number) {
		print_string(id, 2, 8, 1 + 0x10 * number, 1, 0, choices[number]);
	}

	// Field
	current = multichoice_field(2, 1);

	// Allow moving the selecty thing
	multichoice_prepare(id, 2, 0, 1, current + 2, number, 0);

	rboxid_to_vram(id, 3);

	return id;
}

u16 showYesNo() {
	char *choices[] = { caYes, caNo, 0 };

	rbox box = {0, 2, 2, 4, 4, 0xF, 0x130};
	return showMultichoice(&box, choices);
}

void hideMultichoice(u16 id) {
	free_box(1);
	erase_box(id);
	destroy_box(0, 1);
}
