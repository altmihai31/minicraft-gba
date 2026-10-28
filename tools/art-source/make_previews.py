"""Write docs/previews/*.png from the installed art in res/.

usage: python tools/art-source/make_previews.py
"""
import os, sys
from PIL import Image, ImageDraw, ImageFont
sys.path.insert(0, os.path.dirname(__file__))
from art_mobs import MOBS, EXTRAS
from art_items import ICONS
from art_blocks_v2 import connected_tiles, CONNECTED, CONNECTED_CELL

ROOT = os.path.join(os.path.dirname(__file__), '..', '..')
RES, OUT = os.path.join(ROOT, 'res'), os.path.join(ROOT, 'docs', 'previews')
MAG = (255, 0, 255)
FONT = ImageFont.load_default()

def checker(w, h, s=6):
    im = Image.new('RGB', (w, h))
    d = ImageDraw.Draw(im)
    for y in range(0, h, s):
        for x in range(0, w, s):
            d.rectangle([x, y, x + s - 1, y + s - 1],
                        fill=(58, 58, 64) if (x // s + y // s) % 2 else (48, 48, 54))
    return im

def place(sheet, src, box, pos, scale):
    t = src.crop(box).convert('RGBA')
    px = t.load()
    for y in range(t.size[1]):
        for x in range(t.size[0]):
            if px[x, y][:3] == MAG:
                px[x, y] = (0, 0, 0, 0)
    t = t.resize((t.size[0] * scale, t.size[1] * scale), Image.NEAREST)
    sheet.paste(t, pos, t)

def label(d, xy, text, fill=(230, 230, 230)):
    d.text(xy, text, fill=fill, font=FONT)

# ---------------------------------------------------------------- mobs
mobs = Image.open(f'{RES}/images/mobs.png').convert('RGB')
S, cw, rh = 5, 16 * 5 + 8, 16 * 5 + 14
names = [n for n, _, _ in MOBS]
rows = len(MOBS) + 1
sheet = checker(140 + 5 * cw, rows * rh + 28)
d = ImageDraw.Draw(sheet)
for k, h in enumerate(['front', 'back', 'side', 'walk A', 'walk B']):
    label(d, (140 + k * cw, 6), h, (200, 200, 200))
for i in range(len(MOBS) * 5 + len(EXTRAS)):
    row, col = i // 5, i % 5
    y = 22 + row * rh
    if col == 0:
        label(d, (6, y + 34), names[row] if row < len(MOBS) else 'extras')
    place(sheet, mobs, (col * 16, row * 16, col * 16 + 16, row * 16 + 16),
          (140 + col * cw, y), S)
    if row >= len(MOBS):
        label(d, (140 + col * cw, y + 16 * S), EXTRAS[col][0][:16], (170, 170, 170))
sheet.save(f'{OUT}/preview_mobs.png')

# ---------------------------------------------------------------- items
items = Image.open(f'{RES}/images/items.png').convert('RGB')
S, cw, per = 6, 120, 5
sheet = checker(per * cw + 10, -(-len(ICONS) // per) * (8 * S + 18) + 10)
d = ImageDraw.Draw(sheet)
for k, icon in enumerate(ICONS):
    idx = 55 + k
    x, y = 6 + (k % per) * cw, 6 + (k // per) * (8 * S + 18)
    place(sheet, items, ((idx % 5) * 8, (idx // 5) * 8, (idx % 5) * 8 + 8, (idx // 5) * 8 + 8),
          (x, y), S)
    label(d, (x, y + 8 * S + 2), f'{idx} {icon[0]}'[:19])
sheet.save(f'{OUT}/preview_items.png')

# ---------------------------------------------------------------- blocks
level = Image.open(f'{RES}/images/level.png').convert('RGB')
BLOCK_NAMES = ['coal ore', 'obsidian', 'planks', 'stone bricks', 'clay bricks',
               'door closed', 'door open', 'pumpkin', 'melon', 'sugar cane',
               'mushrooms', 'torch', 'spawner', 'ice']
S, cw = 4, 16 * 4 + 40
WALL_ROW = 32 * 2 + 46
sheet = checker(7 * cw + 10, 6 + 2 * (16 * S + 20) + 3 * WALL_ROW)
d = ImageDraw.Draw(sheet)
for k, n in enumerate(BLOCK_NAMES):
    cell = 22 + k
    x, y = 6 + (k % 7) * cw, 6 + (k // 7) * (16 * S + 20)
    place(sheet, level, ((cell % 11) * 16, (cell // 11) * 16,
                         (cell % 11) * 16 + 16, (cell // 11) * 16 + 16), (x, y), S)
    label(d, (x, y + 16 * S + 2), f'{cell} {n}')

# connected building blocks: a 4x2 wall drawn the way mod_tile_draw does it
WALL = ['XXXX', 'X..X']
STATES = {(0, 0, 0): 0, (1, 0, 0): 1, (0, 1, 0): 2, (1, 1, 1): 3, (1, 1, 0): 4}
def is_x(x, y):
    return 0 <= y < len(WALL) and 0 <= x < len(WALL[0]) and WALL[y][x] == 'X'
y0 = 6 + 2 * (16 * S + 20)
for b, (name, tex, row) in enumerate(CONNECTED):
    tiles = connected_tiles(tex)
    wall = Image.new('RGB', (64, 32), MAG)
    for ty in range(2):
        for tx in range(4):
            if not is_x(tx, ty):
                continue
            for q in range(4):
                sx, sy = (1 if q % 2 else -1), (1 if q // 2 else -1)
                v, h = is_x(tx, ty + sy), is_x(tx + sx, ty)
                diag = is_x(tx + sx, ty + sy)
                t = tiles[q * 5 + STATES.get((v, h, diag if v and h else 0), 4)]
                for yy in range(8):
                    for xx in range(8):
                        c = t[yy][xx]
                        wall.putpixel((tx * 16 + (q % 2) * 8 + xx, ty * 16 + (q // 2) * 8 + yy),
                                      tuple(int(c[i:i + 2], 16) for i in (0, 2, 4)))
    y = y0 + b * WALL_ROW
    place(sheet, wall, (0, 0, 64, 32), (6, y), 2)
    label(d, (6 + 64 * 2 + 10, y + 26),
          f'{name}, placed (cells {CONNECTED_CELL + b * 5}-{CONNECTED_CELL + b * 5 + 4})')
sheet.save(f'{OUT}/preview_blocks.png')

# ---------------------------------------------------------------- HUD
gui = Image.open(f'{RES}/images/gui.png').convert('RGB')
g = checker(16 * 10 + 20, 16 * 10 + 20)
place(g, gui, (0, 16, 16, 32), (10, 10), 10)
g.save(f'{OUT}/preview_hud.png')
print('wrote', sorted(os.listdir(OUT)))
