"""Original block tiles for the Minicraft GBA fork.
Writes new level.png + background.png into OUT dir, plus previews."""
import sys, os
from PIL import Image, ImageDraw, ImageFont

SRC, OUT = sys.argv[1], sys.argv[2]
os.makedirs(OUT, exist_ok=True)
MAG = (255, 0, 255)

def hx(s): return tuple(int(s[i:i+2], 16) for i in (0, 2, 4))

# ---------- palette additions (free slots only) ----------
ADD = {
    1:  ['63a563', '8cd68c'],                                  # melon on grass
    5:  ['de7b29', '9c4a10', '63a563', '8cd68c', '317331', '4ac64a'],  # pumpkin
    7:  ['080808', '393939', '9c2929', 'de7b29'],              # spawner
    10: ['846b6b', '523939', 'de7b29', 'f7f7f7'],              # mushrooms, torch
}

# grass speckle positions copied from the game's grass cell (x, y)
SPECK = [(2,2),(11,2),(5,3),(1,4),(13,5),(13,9),(2,10),(6,11),(10,12),(3,14)]

def bg(base, speck=None):
    g = [[base]*16 for _ in range(16)]
    if speck:
        for x, y in SPECK: g[y][x] = speck
    return g

def overlay(g, rows, legend):
    assert len(rows) == 16, len(rows)
    for y, r in enumerate(rows):
        assert len(r) == 16, (y, r, len(r))
        for x, ch in enumerate(r):
            if ch != '.': g[y][x] = legend[ch]
    return g

T = {}  # name -> (grid of hex, intended row)

# 1 COAL ORE ------------------------------------------------------------
T['coal_ore'] = overlay(bg('846b6b'), [
    "................",
    "..KKK.......D...",
    ".KWLGK......KK..",
    ".KLGGK.....KWLK.",
    ".KGGGK....KLGGGK",
    "..KKKD....KGGGGK",
    "...DD......KKKKD",
    ".D..........DDD.",
    ".....KKKK.......",
    "....KWLLGK......",
    "...KLLGGGGK...D.",
    "...KGGGGGGK..KK.",
    "....KGGGKKD.KWLK",
    ".....KKKDD..KGGK",
    "......DDD....KKD",
    "..D...........DD"],
    dict(K='080808', G='393939', L='949494', W='c6c6c6', D='523939'))

# 2 OBSIDIAN ------------------------------------------------------------
T['obsidian'] = overlay(bg('291029'), [
    "................",
    ".VV.......K.....",
    ".V.......K......",
    "........K.......",
    "...K...K....U...",
    "....K.KVV...U...",
    ".....K..........",
    "....K......K....",
    "...K......K.VV..",
    "..K..S...K..V...",
    ".K......K.......",
    "...UU..K........",
    "...U..K......K..",
    "......K.....K...",
    ".............V..",
    "KKKKKKKKKKKKKKKK"],
    dict(K='080808', V='8c298c', S='e7cee7', U='42429c'))

# 3 PLANKS --------------------------------------------------------------
def planks():
    g = bg('ad944a')
    seams = [[6], [2, 11], [8], [4, 13]]
    grain = [(9,1),(10,1),(11,1),(13,5),(14,5),(4,6),(5,6),(11,9),(12,9),(1,13),(2,13),(8,14),(9,14),(10,14)]
    nails = [(7,1),(3,5),(12,5),(9,9),(5,13),(14,13)]
    for b in range(4):
        y0 = 4*b
        for x in range(16):
            g[y0+2][x] = '7b6339' if x % 5 == 2 else 'ad944a'
            g[y0+3][x] = '523939'
        for sx in seams[b]:
            for y in range(y0, y0+3): g[y][sx] = '523939'
    for x, y in grain: g[y][x] = '7b6339'
    for x, y in nails: g[y][x] = '080808'
    return g
T['planks'] = planks()

# 4 STONE BRICKS --------------------------------------------------------
def stone_bricks():
    g = bg('c6c6c6')
    def brick(x0, y0, w, h):
        for y in range(y0, y0+h):
            for x in range(x0, x0+w):
                if not (0 <= x < 16 and 0 <= y < 16): continue
                if y == y0+h-1 or x == x0+w-1: c = '393939'      # mortar
                elif y == y0 or x == x0: c = 'f7f7f7'            # top/left light
                elif y == y0+h-2 or x == x0+w-2: c = '949494'    # bottom/right shade
                else: c = 'c6c6c6'
                g[y][x] = c
    brick(0, 0, 8, 8); brick(8, 0, 8, 8)
    brick(-4, 8, 8, 8); brick(4, 8, 8, 8); brick(12, 8, 8, 8)
    for x, y in [(11,2),(9,12),(1,11),(14,13),(3,10)]:
        g[y][x] = '949494'
    for x, y in [(5,3),(5,4),(4,5)]:
        g[y][x] = '393939'   # small crack
    return g
T['stone_bricks'] = stone_bricks()

# 5 CLAY BRICKS ---------------------------------------------------------
def clay_bricks():
    g = bg('9c2929')
    for c in range(4):
        y0 = 4*c; off = 0 if c % 2 == 0 else 4
        for x in range(16):
            g[y0+2][x] = '421010'
            g[y0+3][x] = 'cea5a5'
        for j in range(off-8, 16, 8):
            jx = (j + 7)
            if 0 <= jx < 16:
                for y in range(y0, y0+3): g[y][jx] = 'cea5a5'
                if jx-1 >= 0: g[y0+2][jx-1] = '211010'
    for x, y in [(2,0),(10,1),(5,5),(13,4),(1,9),(9,8),(6,13),(14,12)]:
        g[y][x] = '421010'
    return g
T['clay_bricks'] = clay_bricks()

# 6 DOOR (closed) -------------------------------------------------------
DOOR_L = dict(K='080808', D='523939', A='ad944a', T='7b6339', Y='b5b55a')
T['door_closed'] = overlay(bg('ad944a'), [
    "KKKKKKKKKKKKKKKK",
    "KDDDDDDDDDDDDDDK",
    "KDYYYTYYYYTYYYDK",
    "KDKAATAAAATAAADK",
    "KDATTTTTTTTTTADK",
    "KDAAATAAAATAAADK",
    "KDAAATAAAATAAADK",
    "KDAAATAAAATAAYDK",
    "KDAAATAAAATAAKDK",
    "KDAAATAAAATAAADK",
    "KDAAATAAAATAAADK",
    "KDATTTTTTTTTTADK",
    "KDKAATAAAATAAADK",
    "KDAAATAAAATAAADK",
    "KDTTTTTTTTTTTTDK",
    "KKKKKKKKKKKKKKKK"], DOOR_L)

# 7 DOOR (open) ---------------------------------------------------------
T['door_open'] = overlay(bg('523939'), [
    "KKKKKKKKKKKKKKKK",
    "KDDDDDDDDDDDDDDK",
    "KDYTKKKKKKKKKKDK",
    "KDATK.........DK",
    "KDATK.........DK",
    "KDATK.....K...DK",
    "KDATK.........DK",
    "KDATK.........DK",
    "KDATK.........DK",
    "KDATK...K.....DK",
    "KDATK.........DK",
    "KDATK.........DK",
    "KDATK......K..DK",
    "KDATK.........DK",
    "KDTTK.........DK",
    "KKKKKKKKKKKKKKKK"], DOOR_L)

# 8 PUMPKIN (on grass) --------------------------------------------------
T['pumpkin'] = overlay(bg('63a563', '8cd68c'), [
    "................",
    "........S.......",
    ".......SSLL.....",
    "....XXXXSXXXX...",
    "...XHHOQXQOOOX..",
    "..XHOOQOOOQOOOX.",
    "..XHOQOOOOOQOOX.",
    ".XHOOQOOOOOQOOOX",
    ".XOOOQOOOOOQOOOX",
    ".XOOOQOOOOOQOOQX",
    ".XOOOQOOOOOQOOQX",
    "..XOOQOOOOOQOQX.",
    "..XQOOQOOOQOQQX.",
    "...XQQQQQQQQQX..",
    "....XXXXXXXXXG..",
    ".....GGGGGGGG..."],
    dict(X='313118', O='de7b29', Q='9c4a10', H='e7e76b', S='317331', L='4ac64a', G='317331'))

# 9 MELON (on grass) ----------------------------------------------------
T['melon'] = overlay(bg('63a563', '8cd68c'), [
    "................",
    "................",
    "....XXXXXXXX....",
    "...XWLMMDMMMDX..",
    "..XWLMDMMMDMMMX.",
    ".XLLMDMMMMDMMDMX",
    ".XLMMDMMMDMMMDMX",
    ".XMMDMMMMDMMMDMX",
    ".XMMDMMMMDMMMDMX",
    ".XMMDMMMMDMMDMMX",
    ".XMMDMMMDMMMDMMX",
    "..XMMDMMDMMMDMX.",
    "...XMDMMDMMDMX..",
    "....XXXXXXXXXD..",
    ".....DDDDDDDD...",
    "................"],
    dict(X='102910', M='429c42', D='215221', L='4ac64a', W='e7e7a5'))

# 10 SUGAR CANE (on grass) ----------------------------------------------
T['sugar_cane'] = overlay(bg('63a563', '8cd68c'), [
    "......PG........",
    "......PG.....PG.",
    "..PG..PG...LLPG.",
    "..PG..PGL....PG.",
    "..PG..JJ.....JJ.",
    "LLPG..PG.....PG.",
    "..JJ..PG.....PGL",
    "..PG.LPG.....PG.",
    "..PG..PGLL...PG.",
    "..PGL.JJ.....JJ.",
    "..PG..PG....LPG.",
    "..JJ..PG.....PG.",
    ".LPG..PG.....PG.",
    "..PG..PG.....PG.",
    ".XXXXXXXX...XXXX",
    "................"],
    dict(P='8cd68c', G='4ac64a', J='317331', L='6bce6b', X='317331'))

# 11 MUSHROOMS (on dirt) ------------------------------------------------
T['mushrooms'] = overlay(bg('846b6b', '523939'), [
    "................",
    "....ZZZZZZ......",
    "...ZRRWRRRZ.....",
    "..ZRWWRRRWRZ....",
    ".ZRRRRRRRWWRZ...",
    ".ZRWRRRWRRRRZ...",
    ".ZRRRRRRRRRRZ...",
    "..ZMMMMMMMMZ....",
    "....ZCCCZ.......",
    "....ZCCMZ..ZZZZ.",
    "....ZCCMZ.ZRWRRZ",
    "....ZCCMZ.ZRRRWZ",
    "....ZCCMZD.ZMMZ.",
    "...ZZCCCMZD.ZCZD",
    "....DZZZZD..ZCZD",
    ".....DDDD....DD."],
    dict(Z='211010', R='9c2929', W='f7f7f7', M='421010', C='cea5a5', D='523939'))

# 12 TORCH (on dirt) ----------------------------------------------------
T['torch'] = overlay(bg('846b6b', '523939'), [
    ".......YY.......",
    "......YOOY......",
    ".....YOOWOY.....",
    ".....YOWWOY.....",
    ".....YOWWOY.....",
    "......ROOR......",
    "......ZRRZ......",
    "......ZNNZ......",
    "......ZNEZ......",
    "......ZNEZ......",
    "......ZNEZ......",
    "......ZNEZ......",
    "......ZNEZ......",
    ".....DZEEZD.....",
    "......DDDD......",
    "................"],
    dict(Y='e7e76b', O='de7b29', W='f7f7f7', R='9c2929', Z='211010', N='636331', E='313118', D='523939'))

# 13 SPAWNER ------------------------------------------------------------
T['spawner'] = overlay(bg('080808'), [
    "WWWWWWWWWWWWWWWG",
    "WLLLLLLLLLLLLLLG",
    "WL.D..D..D..D.LG",
    "WL.D..D..D..D.LG",
    "WLGGGGGGGGGGGGLG",
    "WL.D..D..D..D.LG",
    "WL.D.RD..DR.D.LG",
    "WL.DROD..DORD.LG",
    "WL.D.RDRRD..D.LG",
    "WL.D..DOOD..D.LG",
    "WLGGGGGGGGGGGGLG",
    "WL.D..D..D..D.LG",
    "WL.D..D..D..D.LG",
    "WLLLLLLLLLLLLLLG",
    "WGGGGGGGGGGGGGGG",
    "GGGGGGGGGGGGGGGG"],
    dict(W='c6c6c6', L='949494', G='393939', D='636363', R='9c2929', O='de7b29'))

# 14 ICE ----------------------------------------------------------------
T['ice'] = overlay(bg('c6c6de'), [
    "......W.........",
    ".....W..........",
    "....W.......B...",
    "...W.......B....",
    "..W.......BD....",
    ".........B..D...",
    "........B....D..",
    ".......B........",
    "..............W.",
    ".B...........W..",
    "..B.........W...",
    "...BD......W....",
    "....D...........",
    ".....D.....W....",
    "..........W.....",
    ".........W......"],
    dict(W='f7f7f7', B='9494ad', D='63637b'))

ORDER = ['coal_ore', 'obsidian', 'planks', 'stone_bricks', 'clay_bricks',
         'door_closed', 'door_open', 'pumpkin', 'melon', 'sugar_cane',
         'mushrooms', 'torch', 'spawner', 'ice']

# ---------- write palette ----------
pal = Image.open(f'{SRC}/palettes/background.png').convert('RGB')
pp = pal.load()
for row, cols in ADD.items():
    free = [x for x in range(1, 16) if pp[x, row] == MAG]
    assert len(free) >= len(cols), (row, free, cols)
    for x, c in zip(free, cols): pp[x, row] = hx(c)
pal.save(f'{OUT}/background.png')

# ---------- write level.png ----------
lvl = Image.open(f'{SRC}/images/level.png').convert('RGB')
W, H = lvl.size
assert W == 176
first = (W // 16) * (H // 16)
rows_needed = -(-len(ORDER) // 11)
new = Image.new('RGB', (W, H + 16 * rows_needed), MAG)
new.paste(lvl, (0, 0))
np_ = new.load()
cells = {}
for i, name in enumerate(ORDER):
    cell = first + i
    x0, y0 = (cell % 11) * 16, (cell // 11) * 16
    for y in range(16):
        for x in range(16):
            np_[x0 + x, y0 + y] = hx(T[name][y][x])
    cells[name] = cell
new.save(f'{OUT}/level.png')

# ---------- resolve palette rows exactly like res2gba ----------
rows = [set(pp[x, y] for x in range(16)) for y in range(pal.size[1])]
print('cell  8x8-ids   row  name')
for name in ORDER:
    cell = cells[name]
    x0, y0 = (cell % 11) * 16, (cell // 11) * 16
    cols = set(np_[x0 + x, y0 + y] for x in range(16) for y in range(16))
    r = next((i for i, rs in enumerate(rows) if cols <= rs), None)
    print(f'{cell:4d}  {cell*4:3d}-{cell*4+3:<3d}  {r!s:>4}  {name}')
