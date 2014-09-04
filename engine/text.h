#ifndef TEXT_H_
#define TEXT_H_

#include "../types.h"
#include "../oakintro.h"

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

u16 load_rbox(u32);
u16 fix_box(u16);
int outline_box(u16, u8, u16, u8);
int clear_box(u16, u8);

int free_box(u8 id);
int erase_box(u16);
int destroy_box(u8, u8);
u8 multichoice_get_response();


#endif /* TEXT_H_ */
