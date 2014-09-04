#ifndef INPUT_H_
#define INPUT_H_

/*
 * High: Pressed
 * Low: Released
 */

typedef struct keyState {
	u16 A : 1;
	u16 B : 1;
	u16 select : 1;
	u16 start : 1;
	u16 right : 1;
	u16 left : 1;
	u16 up : 1;
	u16 down : 1;
	u16 R : 1;
	u16 L : 1;
} keyState;

keyState *buttons = (keyState *) 0x0300311E;

#endif /* INPUT_H_ */
