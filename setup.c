#include "engine/callback.h"
#include "images/background.h"

typedef enum DMACTRL {
	INCREMENT,
	DECREMENT,
	FIXED,
	RELOAD,
	PROHIBITED = 3
} DMACTRL;

typedef struct DMA {
	u32 src;
	u32 dst;
	u16 word_count;
	/* TODO: Union with
	 * struct control {
		u16 unused : 4;
		DMACTRL dst_ctrl : 2;
		DMACTRL src_ctrl : 2;
		u16 repeat : 1;
		enum type {
			HALFWORD,
			WORD
		} type : 1;
		enum timing {
			IMMEDIATE,
			VBLANK,
			HBLANK,
			SPECIAL
		} timing : 2;
		u16 irq : 1;
		u16 enabled : 1;
	} control : 16;*/
	u16 control;
} DMA;

DMA *DMA0 = (DMA *) 0x040000B0;
DMA *DMA1 = (DMA *) 0x040000BC;
DMA *DMA2 = (DMA *) 0x040000C8;
DMA *DMA3 = (DMA *) 0x040000D4;

u32 *tilemap = (u32 *) 0x203B108;
u8 (*oak_fade)(u16, u8, u8, u8) = (u8 (*)(void)) 0x08070588 + 1;
u8 (*func)(u16) = (u8 (*)(void)) 0x8000B68 + 1;

void callback(u8);

void super_callback_shit() {
	/*
	 * Sets the hardware stuff so that we can render the tutorial.
	 * Also, allows returning from other functions, such as the naming stuff.
	 */

	u32 null = 0;
	u8 state = super->multi_purpose_state_tracker;

	switch(state) {
	case 0:
		/* set vblank handler */

		//super->callback5_vblank = 0;

		/*
		 * Clear the VRAM, PAL RAM and OAM with DMA
		 */

//		DMA3->dst = 0x06000000;
//		DMA3->src = &null;
//		DMA3->word_count = 0xC000;
//		DMA3->control = 0x8100 | 0x80;
//
//		DMA3->dst = 0x07000000;
//		DMA3->src = &null;
//		DMA3->word_count = 0x100;
//		DMA3->control = 0x8500 | 0x80;
//
//		DMA3->dst = 0x05000000;
//		DMA3->src = &null;
//		DMA3->word_count = 0x1FF;
//		DMA3->control = 0x8100 | 0x80;

		/*
		 * Clean up, free, etc.
		 */

		((void (*)(void)) 0x08070529)();
		((void (*)(void)) 0x08087E65)();

		((void (*)(void)) 0x08006B11)();
		((void (*)(void)) 0x080088F1)();
//
		((void (*)(void)) 0x080F6809)();
		break;
	case 1:
		/*
		 * Reset BG states
		 */

		gpu_tile_bg_drop_all_sets(0);
		bg_vram_setup(1, 0x08462E58, 3);

		bgid_set_tilemap(1, *tilemap + 0x1C20);
		bgid_set_tilemap(2, *tilemap + 0x1820);

		bgid_mod_x_offset(1, 0, 0);
		bgid_mod_y_offset(1, 0, 0);

		bgid_mod_x_offset(2, 0, 0);
		bgid_mod_y_offset(2, 0, 0);
		break;
	case 2:
		/*
		 * Clear IO registers
		 */

		display_ioreg_set(0x40, 0);
		display_ioreg_set(0x44, 0);
		display_ioreg_set(0x48, 0);
		display_ioreg_set(0x4A, 0);
		display_ioreg_set(0x50, 0);
		display_ioreg_set(0x52, 0);
		display_ioreg_set(0x54, 0);
		break;
	case 3:
		/*
		 * Reset message box layer
		 */

		((void (*)(void)) 0x08003ECD)();
		((void (*)(void)) 0x080F6C6D)();
		((void (*)(void)) 0x080F6C99)();
		break;

	case 4:
		break;
	case 5:
		break;
	case 6:
		/*
		 * Set where to resume from
		 */

		//task_add((taskFunction) 0x08130465, 0);
		task_add((taskFunction) callback, 0);
		break;
	case 7:
		oak_fade(1, 0, 0x10, 0);

		display_ioreg_set(0, 0x1040);

		gpu_sync_bg_show(0);
		gpu_sync_bg_show(1);
		gpu_sync_bg_show(2);

		func(1);

		/*
		 * VBlank handler
		 */
		super->callback5_vblank = 0x0812EAFC + 1;

		set_callback2((callbackFunction) 0x0812EB11);
		return;
	}

	/* Do the next action */
	super->multi_purpose_state_tracker = state + 1;
}
