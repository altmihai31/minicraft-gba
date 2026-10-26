import sys, os, json
from PIL import Image
sys.path.insert(0, os.path.dirname(__file__))
from art_items import ICONS, ITEMS_ADD
from art_mobs import MOBS, EXTRAS, SPRITE_ROWS, FURNITURE_ADD

SRC, BLOCKS, OUT = sys.argv[1], sys.argv[2], sys.argv[3]   # res dir, newblocks dir, output dir
os.makedirs(OUT, exist_ok=True)
MAG = (255, 0, 255)
def hx(s): return tuple(int(s[i:i+2], 16) for i in (0, 2, 4))

def paint(img, x0, y0, rows, legend):
    px = img.load()
    for y, r in enumerate(rows):
        for x, ch in enumerate(r):
            px[x0 + x, y0 + y] = MAG if ch == '.' else hx(legend[ch])

def check(rows, n):
    assert len(rows) == n, (len(rows), rows)
    for r in rows: assert len(r) == n, (len(r), r)

def add_colours(pal, adds):
    pp = pal.load()
    for row, cols in adds.items():
        free = [x for x in range(1, 16) if pp[x, row] == MAG]
        assert len(free) >= len(cols), (row, free, cols)
        for x, c in zip(free, cols): pp[x, row] = hx(c)

def resolve(img, box, pal):
    pp = pal.load()
    rows = [set(pp[x, y] for x in range(16)) for y in range(pal.size[1])]
    cols = set(img.crop(box).get_flattened_data())
    return next((i for i, rs in enumerate(rows) if cols <= rs), None)

report = {}

# ------------------------------------------------ items
ipal = Image.open(f'{SRC}/palettes/items.png').convert('RGB')
add_colours(ipal, ITEMS_ADD)
ipal.save(f'{OUT}/items_palette.png')
items = Image.open(f'{SRC}/images/items.png').convert('RGB')
n0 = (items.size[0] // 8) * (items.size[1] // 8)          # 55 existing icon slots
rows_needed = -(-(n0 + len(ICONS)) // 5)
new = Image.new('RGB', (40, rows_needed * 8), MAG); new.paste(items, (0, 0))
report['items'] = []
for i, (name, grid, legend) in enumerate(ICONS):
    check(grid, 8)
    idx = n0 + i
    x0, y0 = (idx % 5) * 8, (idx // 5) * 8
    paint(new, x0, y0, grid, legend)
    r = resolve(new, (x0, y0, x0 + 8, y0 + 8), ipal)
    assert r is not None, name
    report['items'].append((idx, name, r))
new.save(f'{OUT}/items.png')

# ------------------------------------------------ sprites palette + mobs sheet
spal_src = Image.open(f'{SRC}/palettes/sprites.png').convert('RGB')
spal = Image.new('RGB', (16, 12), MAG); spal.paste(spal_src, (0, 0))
add_colours(spal, FURNITURE_ADD)
pp = spal.load()
for row, cols in SPRITE_ROWS.items():
    for x, c in enumerate(cols):
        pp[x, row] = MAG if c is None else hx(c)
spal.save(f'{OUT}/sprites_palette.png')

cells = []
for name, frames, legend in MOBS:
    assert len(frames) == 5, name
    for k, f in enumerate(frames):
        cells.append((f'{name} [{["front","back","side","walk A","walk B"][k]}]', f, legend))
for name, f, legend in EXTRAS:
    cells.append((name, f, legend))
mobs = Image.new('RGB', (80, -(-len(cells) // 5) * 16), MAG)
report['mobs'] = []
for i, (name, f, legend) in enumerate(cells):
    check(f, 16)
    x0, y0 = (i % 5) * 16, (i // 5) * 16
    paint(mobs, x0, y0, f, legend)
    r = resolve(mobs, (x0, y0, x0 + 16, y0 + 16), spal)
    assert r is not None, name
    report['mobs'].append((i, name, r))
mobs.save(f'{OUT}/mobs.png')

# ------------------------------------------------ HUD icons (gui.png) + background palette
bpal = Image.open(f'{BLOCKS}/background.png').convert('RGB')
add_colours(bpal, {10: ['c6c6c6', '949494']})
bpal.save(f'{OUT}/background.png')
G = dict(a='080808', B='846b6b', D='523939', O='de7b29', W='f7f7f7', J='cea5a5', d='211010',
         L='c6c6c6', M='949494', e='313118')
hunger = ["aDDDDaaa", "DOBBBDaa", "DBBBBBDa", "DBBBBBDa", "aDBBBDJa", "aaDDDJWa", "aaaaaJWW", "aaaaaaWJ"]
armor = ["MMaaaaMM", "MLMaaMLM", "MLLMMLLM", "aMLLLLMa", "aMLWLLMa", "aMLLLLMa", "aMLLWLMa", "aMMMMMMa"]
def empty(g, c):
    out = []
    for y in range(8):
        row = ''
        for x in range(8):
            if g[y][x] == 'a': row += 'a'; continue
            edge = any(not (0 <= x+dx < 8 and 0 <= y+dy < 8) or g[y+dy][x+dx] == 'a'
                       for dx, dy in ((1,0),(-1,0),(0,1),(0,-1)))
            row += c if edge else 'a'
        out.append(row)
    return out
gui = Image.open(f'{SRC}/images/gui.png').convert('RGB')
g2 = Image.new('RGB', (32, 32), MAG); g2.paste(gui, (0, 0))
paint(g2, 0, 16, hunger, G); paint(g2, 8, 16, empty(hunger, 'd'), G)
paint(g2, 0, 24, armor, G);  paint(g2, 8, 24, empty(armor, 'e'), G)
g2.save(f'{OUT}/gui.png')
report['gui'] = resolve(g2, (0, 16, 16, 32), bpal)

json.dump(report, open(f'{OUT}/report.json', 'w'), indent=1)
print('items:', len(report['items']), 'icons; rows', sorted(set(r for _, _, r in report['items'])))
print('mobs cells:', len(report['mobs']), '; gui cell row', report['gui'])
for i, n, r in report['mobs']:
    if '[front]' in n or '[' not in n: print(f'  cell {i:2d} row {r:2d}  {n}')
