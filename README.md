OakTutorial
===========

Replacement for the Professor Oak introduction tutorial in Pokemon FireRed.

## Building

Modify configure.py to point to your toolchain (dictionary on last line). 
Change the parameter in makefile to point to your FireRed Rom. Then from a terminal run:

```make
make insert
```

You can clean the build with
```make clean```

Add strings to strings.csv, and the make script will generate string.h
