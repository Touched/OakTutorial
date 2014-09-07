#!/usr/bin/python3

import os
import glob
import sys
from PIL import Image

name = sys.argv[1]
num = int(sys.argv[2])
order = [str(i + 1) for i in range(num)]

frames = {}

last_width = None
total_height = 0

last_height = None


for item in glob.glob(os.path.join(name, '*')):
    frame = Image.open(item)
    
    width, height = frame.size
    
    if last_width and last_width != width:
        raise ValueError('Frames must be same width.')
    else:
        last_width = width
        
    if last_height and last_height != height:
        raise ValueError('Frames must be same height.')
    else:
        last_height = height
        
    total_height += height
    
    frame_name = os.path.basename(item)
    frame_name, _ = os.path.splitext(frame_name)
    frames[frame_name] = frame
    
size = last_width, total_height
item_height = last_height

output = Image.new('RGBA', size)

for i, item in enumerate(order):
    y = i * item_height
    box = 0, y, width, y + item_height
    
    output.paste(frames[item], box)
    
output.save(name + '.png')
    
