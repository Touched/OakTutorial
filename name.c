/*
 * Use -fshort-enums
 */

void npc_to_objtemplate(u8 id, u32 *callback, template *output, u32 *field14);

typedef struct namingInstance {
	u8 prefilled; /* Whether the box is automatically filled */
	u8 length; /* The maximum characters a user can enter */
	u8 image_function_index; /* What function to use */
	enum {
		NONE,
		MALE,
		FEMALE
	} gender : 8; /* What gender symbol is displayed */
	u32 unk2; /* Always 1, doesn't appear to do anything */
	char *prompt; /* 0xFF terminated string */
} namingInstance;

typedef void (*nameLoadSprite)(void);

