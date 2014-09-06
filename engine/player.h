#ifndef PLAYER_H_
#define PLAYER_H_

typedef struct playerData {
	char name[8];
	u8 gender;
	u8 unk;
	u16 trainer_id;
	u16 secret_id;
	u16 hours;
	u8 minutes;
	u8 seconds;
	u8 frames;
	u8 unk2;
	u16 options;
} playerData;

playerData **player = (playerData **) 0x0300500C;

#endif
