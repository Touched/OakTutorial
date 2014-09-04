#include "text.h"

void showMessage(char *message) {
	char *dest = (char*) 0x02021D18;
	u8 b = 0;

	loadMessageBox(0, 0);
	fdecoder(dest, message);
	box_related_one(0, 4, dest, b, 0, 2, 1, 3);

	rboxid_to_vram(0, 3);
}

rbox boy_girl = {0, 0x12, 9, 9, 4, 0xF, 0x174};

rbox info_multichoice = {0, 2, 2, 15, 6, 0xF, 0x130};

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
	// id, font, x, y, bg_color, fg_color, str
	int (*print)(u16, u8, u8, u8, u32, u32, u32*) = (int (*)(void)) 0x0812E51C + 1;

	for (number = 0; choices[number]; ++number) {
		print(id, 2, 8, 1 + 0x10 * number, 1, 0, choices[number]);
	}

	// Field
	u8 (*field)(u8, u8) = (u8 (*)(void)) 0x080F79D8 + 1;
	current = field(2, 1);

	// Allow moving the selecty thing
	int (*huh)(u8, u8, u8, u8, u8, u8, u32) = (int (*)(void)) 0x0810F7D8 + 1;
	huh(id, 2, 0, 1, current + 2, number, 0);

	rboxid_to_vram(id, 3);

	return id;
}

void hideMultichoice(u16 id) {
	free_box(1);
	erase_box(id);
	destroy_box(0, 1);
}
