.align 2
.thumb
.thumb_func

@ Force the correct method to be loaded. 
@ This s file should appear first in the list of targets
@ Long call method, compiled C gets huge pretty quickly

ldr r1, .callbacks
bx r1

@ Since GCC naively inlines functions, I put this here. This will allow
@ my code to call this function with the proper parameters, rather than
@ just a swi put randomly in the code.
.global bitUnPack
bitUnPack:
	swi #0x10

.align 2
.callbacks: .word callback
