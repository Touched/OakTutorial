#ifndef TEXT_H_
#define TEXT_H_

#include "../types.h"
#include "../oakintro.h"

u32 *textflags = (u32*) 0x03003E50;

typedef struct rbox {
	u8 bgid;
	u8 x;
	u8 y;
	u8 w;
	u8 h;
	u8 pal;
	u16 offset;
} rbox;

void showMessage(char *message);
u16 showMultichoice(rbox *box, char **choices);
u16 showYesNo();
void hideMultichoice(u16 id);

u16 load_rbox(u32);
u16 fix_box(u16);
int outline_box(u16, u8, u16, u8);
int clear_box(u16, u8);

int free_box(u8 id);
void textbox_close();
int erase_box(u16);
int destroy_box(u8, u8);
s8 multichoice_get_response(); /* Returns -1 if no choice yet */

void strcpy(char *dst, char *src);


#endif /* TEXT_H_ */
