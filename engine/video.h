#ifndef VIDEO_H_
#define VIDEO_H_

#include "../types.h"

#define ROTSCALE_EMPTY 0x8231CFC
void (*object_callback_nullsub)(void) = (void (*)(void)) 0x0800760C + 1;

typedef struct frame {
	u16 data;
	u16 duration;
} frame;

typedef struct sprite {
	u16 attr0;
	u16 attr1;
	u16 attr2;
	u16 rotscale;
} sprite;

typedef struct template {
	u16 tiles_tag;
	u16 pal_tag;
	sprite *oam;
	frame **animation;
	u32 graphics;
	u32 rotscale;
	u32 callback;
} template;


typedef struct object {
	sprite final_oam;
	frame **animation_table;
	u32 *gfx_table;
	u32 *rotscale_table;
	template *template;
	u32 field18;
	u32 *callback;
	u16 x;
	u16 y;
	u16 x2;
	u16 y2;
	u8 x_centre;
	u8 y_centre;
	u8 anim_number;
	u8 anim_frame;
	u8 anim_delay;
	u8 counter;
	u16 private[8];
	u8 bitfield2;
	u8 bitfield;
	u16 anim_data_offset;
	u8 field42;
	u8 field43;
} object;

typedef struct resource {
	u32 tiles;
	u16 size;
	u16 tag;
} resource;

#define FRAME_END { 0xFFFF, 0 }
#define FRAME_LOOP { 0xFFFE, 0 }

object *objects = (object *) 0x0202063C;
sprite *oamdata = (sprite*)0x07000000;

u16 object_load_compressed_graphics(u32 *src);
u8 object_load_palette(u32 *src);
u8 object_display(u32 *temp, u16 x, u16 y, u8 b);
void load_palette(u32 *src, u16 dest_offset, u16 length);
void display_ioreg_set(u8 reg, u16 value);

#endif /* OBJECTS_H_ */
