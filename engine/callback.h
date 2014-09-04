#ifndef CALLBACK_H_
#define CALLBACK_H_

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

task *tasks = (task *) 0x3005090;

#endif /* CALLBACK_H_ */
