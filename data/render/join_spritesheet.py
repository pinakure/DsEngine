import os
import sys
from PIL import Image
if len(sys.argv)==1:
    print("Missing mask filename.")
    exit(1)
filemask = sys.argv[1]
width =  64
height = 64
dest = Image.new('RGBA', (width*4, height*15),  (255, 0,255, 0))
x = 0
y = 0
c = 0
for i in range(0,15):
    x=0
    for l in range(0,4):
        mask = Image.open(f'{filemask}00{"0" if c<10 else ""}{str(c)}.png').resize((width,height)).convert('RGBA')
        img = Image.open(f'{filemask}00{"0" if c<10 else ""}{str(c)}.png').resize((width, height))
        dest.paste(img, (x, y), mask)
        x+=width
        c+=1
    y+=height
dest.save(f"spritesheet_{filemask}.png")
print(f"Saved spritesheet_{filemask}.png")