"""Rewrite the mod art in res/ in place from the grids in this folder.

build_art.py made the original art pack by appending to the upstream sheets;
running it again would append a second copy. This script instead overwrites
the mod cells where they already are, and checks that every cell still
resolves to the palette row the game draws it with.

usage: python tools/art-source/update_art.py [preview.png]
"""
import sys, os
from PIL import Image
sys.path.insert(0, os.path.dirname(__file__))
from art_mobs import MOBS, EXTRAS, SPRITE_ROWS
from art_blocks_v2 import BLOCKS, BG_ADD, BG_SET, ICONS

RES = os.path.join(os.path.dirname(__file__), '..', '..', 'res')
MAG = (255, 0, 255)
def hx(s): return tuple(int(s[i:i+2], 16) for i in (0, 2, 4))

def paint(img, x0, y0, rows, legend):
    px = img.load()
    for y, r in enumerate(rows):
        assert len(r) == len(rows), (len(r), r)
        for x, ch in enumerate(r):
            px[x0 + x, y0 + y] = MAG if ch == '.' else hx(legend[ch])

def palette_rows(pal):
    pp = pal.load()
    return [set(pp[x, y] for x in range(16)) for y in range(pal.size[1])]

def resolve(img, box, rows):
    cols = set(img.crop(box).getdata())
    return next((i for i, rs in enumerate(rows) if cols <= rs), None)

def add_colours(pal, adds):
    pp = pal.load()
    for row, cols in adds.items():
        have = set(pp[x, row] for x in range(16))
        for c in cols:
            if hx(c) in have:
                continue
            free = [x for x in range(1, 16) if pp[x, row] == MAG]
            assert free, (row, c)
            pp[free[0], row] = hx(c)
            have.add(hx(c))

# expected sprite palette row of each mobs.png cell: must match the palette
# the code draws with (src/entity/mod-mobs.c, projectiles.c, furniture.c)
MOB_ROWS = {'cow': 8, 'pig': 8, 'chicken': 9, 'sheep': 9, 'sheep (sheared)': 9,
            'wolf': 10, 'skeleton': 10, 'spider': 10, 'bloater': 11, 'magma golem': 11,
            'arrow (horizontal)': 9, 'arrow (vertical)': 9, 'XP orb': 11,
            'bed': 6, 'enchanting table': 6}

# ------------------------------------------------------------ mobs.png
# rows 8-11 are mod-only: rewrite them from art_mobs.SPRITE_ROWS
spal = Image.open(f'{RES}/palettes/sprites.png').convert('RGB')
sp = spal.load()
for row, cols in SPRITE_ROWS.items():
    for x, c in enumerate(cols):
        sp[x, row] = MAG if c is None else hx(c)
spal.save(f'{RES}/palettes/sprites.png')
srows = palette_rows(spal)

cells = []
for name, frames, legend in MOBS:
    assert len(frames) == 5, name
    cells += [(name, f, legend) for f in frames]
cells += list(EXTRAS)

mobs = Image.new('RGB', (80, -(-len(cells) // 5) * 16), MAG)
for i, (name, f, legend) in enumerate(cells):
    x0, y0 = (i % 5) * 16, (i // 5) * 16
    paint(mobs, x0, y0, f, legend)
    r = resolve(mobs, (x0, y0, x0 + 16, y0 + 16), srows)
    assert r == MOB_ROWS[name], (i, name, r)
mobs.save(f'{RES}/images/mobs.png')

# ------------------------------------------------------------ level.png
bpal = Image.open(f'{RES}/palettes/background.png').convert('RGB')

# Adding colours to a row must not move any other tile to that row:
# compare every 8x8 tile of the BG images before and after.
BG_IMAGES = ['level', 'logo', 'font', 'gui']
def bg_tile_rows(rows):
    out = {}
    for name in BG_IMAGES:
        img = Image.open(f'{RES}/images/{name}.png').convert('RGB')
        for ty in range(img.size[1] // 8):
            for tx in range(img.size[0] // 8):
                if name == 'level' and (tx // 2) + (ty // 2) * 11 in BLOCKS:
                    continue
                box = (tx * 8, ty * 8, tx * 8 + 8, ty * 8 + 8)
                cols = set(img.crop(box).getdata()) - {MAG}
                if cols:
                    out[(name, tx, ty)] = next(
                        (i for i, rs in enumerate(rows) if cols <= rs), None
                    )
    return out

before = bg_tile_rows(palette_rows(bpal))
add_colours(bpal, BG_ADD)
bp = bpal.load()
for row, cols in BG_SET.items():
    for x, c in cols.items():
        assert bp[x, row] in (MAG, hx(c)), (row, x, bp[x, row])
        bp[x, row] = hx(c)
brows = palette_rows(bpal)
after = bg_tile_rows(brows)
moved = [k for k in before if before[k] != after[k]]
assert not moved, ('tiles would change palette row', moved)
bpal.save(f'{RES}/palettes/background.png')

# res2gba stores every 8x8 tile relative to the first row holding its
# colours; drawing it with another row is fine only if each colour sits at
# the same index there
def tile_draws_right(img, box, row, rows, pal_px):
    cols = set(img.crop(box).getdata()) - {MAG}
    first = next((i for i, rs in enumerate(rows) if cols <= rs), None)
    if first is None:
        return False
    idx = {pal_px[x, first]: x for x in range(15, -1, -1)}
    return all(pal_px[idx[c], row] == c for c in cols)

level = Image.open(f'{RES}/images/level.png').convert('RGB')
height = (max(BLOCKS) // 11 + 1) * 16
if level.size[1] < height:
    grown = Image.new('RGB', (level.size[0], height), MAG)
    grown.paste(level, (0, 0))
    level = grown
bp = bpal.load()
for cell, (name, grid, row) in BLOCKS.items():
    x0, y0 = (cell % 11) * 16, (cell // 11) * 16
    px = level.load()
    for y in range(16):
        for x in range(16):
            px[x0 + x, y0 + y] = hx(grid[y][x])
    r = resolve(level, (x0, y0, x0 + 16, y0 + 16), brows)
    assert r == row, (cell, name, r)
    for q in range(4):
        tx, ty = x0 + (q % 2) * 8, y0 + (q // 2) * 8
        assert tile_draws_right(level, (tx, ty, tx + 8, ty + 8), row, brows, bp), \
            (cell, name, q)
level.save(f'{RES}/images/level.png')

# ------------------------------------------------------------ items.png
ipal = Image.open(f'{RES}/palettes/items.png').convert('RGB')
irows = palette_rows(ipal)
items = Image.open(f'{RES}/images/items.png').convert('RGB')
for icon, (name, grid, legend, row) in ICONS.items():
    x0, y0 = (icon % 5) * 8, (icon // 5) * 8
    paint(items, x0, y0, grid, legend)
    r = resolve(items, (x0, y0, x0 + 8, y0 + 8), irows)
    assert r == row, (icon, name, r)
items.save(f'{RES}/images/items.png')

print('mobs.png:', len(cells), 'cells; level.png cells', sorted(BLOCKS),
      '; items.png icons', sorted(ICONS))

if len(sys.argv) > 1:
    prev = mobs.copy()
    pp = prev.load()
    for y in range(prev.size[1]):
        for x in range(prev.size[0]):
            if pp[x, y] == MAG:
                pp[x, y] = (90, 90, 110) if ((x // 16 + y // 16) & 1) else (70, 70, 90)
    prev.resize((prev.size[0] * 5, prev.size[1] * 5), Image.NEAREST).save(sys.argv[1])
