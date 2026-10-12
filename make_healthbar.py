# tools/make_healthbar.py
from PIL import Image

FRAMES = 10          # 0..8 segmenti verdi
W, H = 16, 8
img = Image.new("P", (W, H * FRAMES), 0)
pal = [255, 0, 255,   # 0 = trasparente (magenta)
       0, 0, 0,       # 1 = bordo nero
       0, 200, 0,     # 2 = verde
       220, 0, 0]     # 3 = rosso
img.putpalette(pal + [0] * (768 - len(pal)))

for f in range(FRAMES):
    y0 = f * H
    # bordo
    for x in range(W):
        img.putpixel((x, y0 + 2), 1); img.putpixel((x, y0 + 5), 1)
    for y in range(2, 6):
        img.putpixel((0, y0 + y), 1); img.putpixel((W - 1, y0 + y), 1)
    # interno 14x2 px: 14 px -> f/8 verde
    green = (14 * f) // (FRAMES - 1)
    for x in range(14):
        c = 2 if x < green else 3
        img.putpixel((1 + x, y0 + 3), c); img.putpixel((1 + x, y0 + 4), c)

img.save("graphics/healthbar.bmp")