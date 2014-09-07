#ifndef CALLBACK_H_
#define CALLBACK_H_

#include "video.h"

typedef struct superstate {
	u32 callback1;
	u32 callback2;
	u32 callback2backup;
	u32 callback5_vblank;
	u32 hblank_callback;
	u32 field_14;
	u32 field_18;
	u32 bits_to_wait_for;
	u32 ptr_vblank_counter;
	u32 field_24;
	u16 buttons0_held;
	u16 buttons1_new;
	u16 buttons2_held_remapped;
	u16 buttons3_new_remapped;
	u16 buttons4_new_and_keyrepeat;
	u16 keypad_countdown;
	u32 unused_apparently;
	sprite sprites[128];
	u8 multi_purpose_state_tracker;
	u8 gpu_sprites_upload_skip;
} superstate;

typedef struct task {
	u32 function;
	u8 in_use;
	u8 prev;
	u8 next;
	u8 priority;
	u16 args[16];
} task;

task *tasks = (task *) 0x3005090;
superstate *super = (superstate *) 0x030030F0;

typedef void (*taskFunction)(u8);
typedef void (*callbackFunction)(void);

u8 task_add(taskFunction func, u8 priority);
u8 set_callback2(callbackFunction func);

#endif /* CALLBACK_H_ */
