#ifndef OAK_INTRO_H_
#define OAK_INTRO_H_

#include "types.h"

void loadMessageBox(u8 b1, u8 b2);
char *fdecoder(char *dest, char *src);
void box_related_one(u8 b1, u8 b2, char *str, u8 b3, u8 b4, u8 b5, u8 b6, u8 b7);
void rboxid_to_vram(u8 rbox_id, u8 tileset);

void fadescreen(int bitmask, u8 r1, u8 r2, u8 r3, u16 color); 

void *malloc_and_LZ77UnComp(void *src, u32 *size);
void *gpu_copy_to_tileset(u8 bgid, void *src, u16 size, u16 offset);
void gpu_copy_tilemap(u8 bgid, void *src, u16 raw, u16 something);
void bgid_send_tilemap(u8 bgid);
void song_play_for_text(u16 song);

void song_fadeout();

void LZ77UnCompVram(void *src, void *dst);
void gpu_pal_apply(void *src, u8 b1, u8 b2);
void *malloc_and_clear(u32 size);
void bgid_fill_rectangle(u8 bgid, u16 x, u8 y, u8 y2, u8 b1, u8 b2, u8 b3);
void free(void *ptr);


#endif
