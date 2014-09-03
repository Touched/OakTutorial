.align 2
.thumb

@ Force the correct method to be loaded. 
@ This s file should appear first in the list of targets
@ Long call method, compiled C gets huge pretty quickly

ldr r1, .callbacks
bx r1

.align 2
.callbacks: .word callback
