"""Revised mod blocks and item icons, written in place by update_art.py.

- clay bricks use exactly the stone-brick pattern, in red
- doors lose their black frame: dark brown edges like the original tiles
- stone-brick and brick icons: the same brick pattern, dark mortar instead
  of a black box outline
"""

def stone_pattern(light, base, shade, mortar, crack=True):
    """The stone-brick tile: two rows of 8x8 bricks, the lower row offset."""
    g = [[base] * 16 for _ in range(16)]
    def brick(x0, y0, w, h):
        for y in range(y0, y0 + h):
            for x in range(x0, x0 + w):
                if not (0 <= x < 16 and 0 <= y < 16):
                    continue
                if y == y0 + h - 1 or x == x0 + w - 1: c = mortar
                elif y == y0 or x == x0:               c = light
                elif y == y0 + h - 2 or x == x0 + w - 2: c = shade
                else:                                    c = base
                g[y][x] = c
    brick(0, 0, 8, 8); brick(8, 0, 8, 8)
    brick(-4, 8, 8, 8); brick(4, 8, 8, 8); brick(12, 8, 8, 8)
    for x, y in [(11, 2), (9, 12), (1, 11), (14, 13), (3, 10)]:
        g[y][x] = shade
    if crack:
        for x, y in [(5, 3), (5, 4), (4, 5)]:
            g[y][x] = mortar
    return [''.join('%s,' % c for c in r) for r in g]

def from_legend(rows, legend):
    return [''.join('%s,' % legend[ch] for ch in r) for r in rows]

DOOR = dict(D='523939', T='7b6339', A='ad944a', Y='b5b55a', K='080808', X='846b6b')

door_closed = from_legend([
    "DDDDDDDDDDDDDDDD",
    "DTTTTTTTTTTTTTTD",
    "DTYYYYYYYYYYYYTD",
    "DTKAATAAAATAAATD",
    "DTATTTTTTTTTTATD",
    "DTAAATAAAATAAATD",
    "DTAAATAAAATAAATD",
    "DTAAATAAAATAAYTD",
    "DTAAATAAAATAAKTD",
    "DTAAATAAAATAAATD",
    "DTAAATAAAATAAATD",
    "DTATTTTTTTTTTATD",
    "DTKAATAAAATAAATD",
    "DTAAATAAAATAAATD",
    "DTTTTTTTTTTTTTTD",
    "DDDDDDDDDDDDDDDD"], DOOR)

door_open = from_legend([
    "DDDDDDDDDDDDDDDD",
    "DTTTTTTTTTTTTTTD",
    "DTYADDDDDDDDDDTD",
    "DTATXXXXXXXXXXTD",
    "DTATXXXXXXXDXXTD",
    "DTATXXDXXXXXXXTD",
    "DTATXXXXXXXXXXTD",
    "DTATXXXXXXXXXXTD",
    "DTATXXXXXDXXXXTD",
    "DTATXXXXXXXXXXTD",
    "DTATXXXDXXXXXXTD",
    "DTATXXXXXXXXXDTD",
    "DTATXXXXXXXXXXTD",
    "DTATXDXXXXXXXXTD",
    "DTTTTTTTTTTTTTTD",
    "DDDDDDDDDDDDDDDD"], DOOR)

def hexgrid(rows):
    return [r.rstrip(',').split(',') for r in rows]

def planks_pattern():
    """The planks tile (same as blocks.py): four boards with seams and nails."""
    g = [['ad944a'] * 16 for _ in range(16)]
    seams = [[6], [2, 11], [8], [4, 13]]
    for b in range(4):
        y0 = 4 * b
        for x in range(16):
            g[y0 + 2][x] = '7b6339' if x % 5 == 2 else 'ad944a'
            g[y0 + 3][x] = '523939'
        for sx in seams[b]:
            for y in range(y0, y0 + 3):
                g[y][sx] = '523939'
    for x, y in [(9, 1), (10, 1), (11, 1), (13, 5), (14, 5), (4, 6), (5, 6), (11, 9),
                 (12, 9), (1, 13), (2, 13), (8, 14), (9, 14), (10, 14)]:
        g[y][x] = '7b6339'
    for x, y in [(7, 1), (3, 5), (12, 5), (9, 9), (5, 13), (14, 13)]:
        g[y][x] = '080808'
    return g

CLAY_BRICKS = hexgrid(stone_pattern('cea5a5', '9c2929', '6b2121', '211010'))

# ----------------------------------------------------------------------
# Placed building blocks join their own kind like rock does (square edges,
# 28.10.2026). Each block gets 20 8x8 tiles: tile = base + quadrant * 5 +
# state, quadrants 0 TL, 1 TR, 2 BL, 3 BR. For a quadrant, v/h say whether
# the neighbour above/below (v) or left/right (h) on its side is the same
# block, diag the one on its corner:
#   state 0: !v !h   1: v !h   2: !v h   3: v h diag (plain)   4: v h !diag
# mod_tile_draw (src/mod-tiles.c) picks the same states.
# Look: white rim on exposed top/left sides, a dark face on the south side
# (the texture two shades darker), black outline, ground (transparent,
# shows the level's dirt colour like rock) outside.
CONNECTED_CELL = 36  # cells 36-50 = tiles 144-203: planks, stone bricks, clay bricks
STATES = [(0, 0, 0), (1, 0, 0), (0, 1, 0), (1, 1, 1), (1, 1, 0)]

def square_template(q, v, h, diag):
    """Classes per pixel: ? ground, L texture, W rim, D face/outline."""
    g = [['L'] * 8 for _ in range(8)]
    top, left = q in (0, 1), q in (0, 2)
    if v and h and diag:
        return g
    ex_top, ex_bot = top and not v, (not top) and not v
    ex_left, ex_right = left and not h, (not left) and not h
    for y in range(8):
        for x in range(8):
            if ex_top and y == 0: g[y][x] = '?'
            elif ex_top and y == 1: g[y][x] = 'W'
            if ex_left and x == 0: g[y][x] = '?'
            elif ex_left and x == 1 and g[y][x] == 'L': g[y][x] = 'W'
            if ex_right and x >= 6: g[y][x] = 'D' if x == 6 else '?'
            if ex_bot and y >= 2: g[y][x] = 'D' if y < 7 else '?'
    if v and h:  # inner corner: the diagonal neighbour is missing
        if q == 0: g[0][0] = 'D'; g[0][1] = g[1][0] = 'W'
        if q == 1: g[0][7] = 'D'
        if q == 2: g[7][0] = 'D'
        if q == 3: g[7][7] = 'D'
    if ex_top and ex_left: g[1][1] = 'W'
    return g

def lum(c):
    r, g, b = (int(c[i:i + 2], 16) for i in (0, 2, 4))
    return r * 3 + g * 6 + b

def connected_tiles(tex):
    """20 8x8 tiles (hex grids) of a 16x16 texture, in tile order."""
    ramp = sorted({c for r in tex for c in r}, key=lum)
    darker = {c: ramp[max(0, i - 2)] for i, c in enumerate(ramp)}
    tiles = []
    for q in range(4):
        ox, oy = (q % 2) * 8, (q // 2) * 8
        for v, h, diag in STATES:
            t = square_template(q, v, h, diag)
            out = [[None] * 8 for _ in range(8)]
            for y in range(8):
                for x in range(8):
                    k, c = t[y][x], tex[oy + y][ox + x]
                    if k == '?':
                        c = 'ff00ff'
                    elif k == 'W':
                        c = ramp[-1]
                    elif k == 'D':
                        edge = any(0 <= x + dx < 8 and 0 <= y + dy < 8 and t[y + dy][x + dx] == '?'
                                   for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)))
                        c = ramp[0] if edge else darker[c]
                    out[y][x] = c
            tiles.append(out)
    return tiles

def tiles_to_cells(tiles):
    """Pack 8x8 tiles into 16x16 cells: tile n -> cell n // 4, quadrant n % 4."""
    cells = []
    for c in range(len(tiles) // 4):
        g = [[None] * 16 for _ in range(16)]
        for q in range(4):
            t = tiles[c * 4 + q]
            for y in range(8):
                for x in range(8):
                    g[(q // 2) * 8 + y][(q % 2) * 8 + x] = t[y][x]
        cells.append(g)
    return cells

CONNECTED = [('planks', planks_pattern(), 0),
             ('stone bricks', hexgrid(stone_pattern('f7f7f7', 'c6c6c6', '949494', '393939')), 2),
             ('clay bricks', CLAY_BRICKS, 9)]

# level.png cell -> (name, 16x16 grid of hex colours, BG palette row)
BLOCKS = {
    26: ('clay bricks', CLAY_BRICKS, 9),
    27: ('door closed', hexgrid(door_closed), 0),
    28: ('door open',   hexgrid(door_open), 0),
}
for b, (name, tex, row) in enumerate(CONNECTED):
    for i, g in enumerate(tiles_to_cells(connected_tiles(tex))):
        BLOCKS[CONNECTED_CELL + b * 5 + i] = (f'{name} (connected {i})', g, row)

# ----------------------------------------------------------------------
# Torches stand on the ground they were placed on (28.10.2026): the tile
# data byte keeps it (0 dirt, 1 grass, 2 sand; see src/mod-tiles.c), and
# each variant uses the real ground texture (the grass/dirt speckles), so
# the torch blends with the tiles around it.
# - dirt: BG row 3, whose indices 12/13 the game sets to the level's dirt
#   colours (grey underground, brown on the surface)
# - grass and sand: BG row 5 (pumpkin row: grass, sand and flame colours)
TORCH = [
    ".......YY.......",
    "......YRRY......",
    ".....YRRWRY.....",
    ".....YRWWRY.....",
    ".....YRWWRY.....",
    "......RRRR......",
    "......ORRO......",
    "......OSSO......",
    "......OSDO......",
    "......OSDO......",
    "......OSDO......",
    "......OSDO......",
    "......OSDOh.....",
    ".....hODDOh.....",
    "......hhhh......",
    "................"]
GROUND_SPECKS = [(2, 2), (11, 2), (5, 3), (1, 4), (13, 5), (13, 9), (2, 10), (6, 11), (10, 12), (3, 14)]
FLAME = dict(O='080808', Y='e7e76b', R='de7b29', W='f7f7f7')

def torch_on(ground, speck, legend):
    g = [[legend.get(ch, ground) for ch in row] for row in TORCH]
    for x, y in GROUND_SPECKS:
        if TORCH[y][x] == '.':
            g[y][x] = speck
    return g

TORCH_CELLS = {
    33: ('torch on dirt', torch_on('846b6b', '523939',
                                   dict(FLAME, S='7b6339', D='4a3121', h='523939')), 3),
    51: ('torch on grass', torch_on('63a563', '8cd68c',
                                    dict(FLAME, S='9c4a10', D='313118', h='317331')), 5),
    # on sand the yellow flame tip would vanish: orange instead
    52: ('torch on sand', torch_on('e7e76b', 'b5b55a',
                                   dict(FLAME, Y='de7b29', S='9c4a10', D='313118', h='b5b55a')), 5),
}
BLOCKS.update(TORCH_CELLS)

# colours added to free slots of background palette rows
BG_ADD = {9: ['cea5a5', '9c2929', '6b2121', '211010']}

# Colours that must sit at a fixed index. Water draws its shore tiles (24-35)
# with indices 3 (water), 12 (rim) and 13 (bank): row 8 (dirt, which only uses
# 1-2) gets an ice shore there, used by liquid_draw next to ice.
BG_SET = {8: {3: '10108c', 12: '9494ad', 13: 'c6c6de'},
          # torch on dirt; 7b6339 at the same index as in row 0, so an 8x8
          # tile holding only dirt + stick colours (it resolves to row 0)
          # still draws right with row 3
          3: {7: 'de7b29', 8: '7b6339', 9: '4a3121', 10: 'e7e76b', 11: 'f7f7f7'},
          # torch on grass and sand: the flame core
          5: {10: 'f7f7f7'}}

# items.png icon -> (name, 8x8 grid, legend, items palette row)
BRICK_ICON = [
    "........",
    ".aaecaae",
    ".eebceeb",
    ".ccccccc",
    ".aecaaab",
    ".ebceebb",
    ".ccccccc",
    "........"]
ICONS = {
    56: ('stone bricks', BRICK_ICON, dict(a='f7f7f7', e='c6c6c6', b='949494', c='393939'), 1),
    57: ('brick',        BRICK_ICON, dict(a='ce9c6b', e='9c2929', b='631818', c='211010'), 0),
}
