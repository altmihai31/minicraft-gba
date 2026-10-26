import sys, json
from PIL import Image, ImageDraw, ImageFont
OUT = sys.argv[1]; f = ImageFont.load_default()
rep = json.load(open(f'{OUT}/report.json'))
def checker(w, h, s=6):
    im = Image.new('RGB', (w, h)); d = ImageDraw.Draw(im)
    for y in range(0, h, s):
        for x in range(0, w, s):
            d.rectangle([x, y, x+s-1, y+s-1], fill=(58,58,64) if (x//s+y//s) % 2 else (48,48,54))
    return im
def place(sheet, src, box, pos, scale):
    t = src.crop(box).convert('RGBA'); px = t.load()
    for y in range(t.size[1]):
        for x in range(t.size[0]):
            if px[x, y][:3] == (255, 0, 255): px[x, y] = (0, 0, 0, 0)
    t = t.resize((t.size[0]*scale, t.size[1]*scale), Image.NEAREST)
    sheet.paste(t, pos, t)
# mobs sheet
mobs = Image.open(f'{OUT}/mobs.png').convert('RGB')
S = 5; cw = 16*S + 8; W = 160 + 5*cw; rows = 11
sheet = checker(W, rows*(16*S+14) + 30); d = ImageDraw.Draw(sheet)
for k, h in enumerate(['front', 'back', 'side', 'walk A', 'walk B']):
    d.text((160 + k*cw, 6), h, fill=(200,200,200), font=f)
for i, n, r in rep['mobs']:
    row, col = i // 5, i % 5
    y = 22 + row*(16*S+14)
    if col == 0: d.text((6, y + 30), n.split(' [')[0] if i < 50 else 'extras', fill=(235,235,235), font=f)
    place(sheet, mobs, ((i%5)*16, (i//5)*16, (i%5)*16+16, (i//5)*16+16), (160 + col*cw, y), S)
    if i >= 50: d.text((160 + col*cw, y + 16*S), n[:16], fill=(170,170,170), font=f)
sheet.save(f'{OUT}/preview_mobs.png')
# items sheet
items = Image.open(f'{OUT}/items.png').convert('RGB')
S = 6; cw = 120; per = 5
sheet = checker(per*cw + 10, 8*(8*S+18) + 10); d = ImageDraw.Draw(sheet)
for k, (idx, n, r) in enumerate(rep['items']):
    x = 6 + (k % per)*cw; y = 6 + (k // per)*(8*S+18)
    place(sheet, items, ((idx%5)*8, (idx//5)*8, (idx%5)*8+8, (idx//5)*8+8), (x, y), S)
    d.text((x, y + 8*S + 2), f'{idx} {n}'[:19], fill=(225,225,225), font=f)
sheet.save(f'{OUT}/preview_items.png')
# gui
gui = Image.open(f'{OUT}/gui.png').convert('RGB')
g = checker(16*10 + 20, 16*10 + 20); place(g, gui, (0, 16, 16, 32), (10, 10), 10); g.save(f'{OUT}/preview_hud.png')
