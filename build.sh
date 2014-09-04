#!/bin/bash
./image.sh
make clean && make && make insert && vbam test.gba
