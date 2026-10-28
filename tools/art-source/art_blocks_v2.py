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

# level.png cell -> (name, 16x16 grid of hex colours, BG palette row)
BLOCKS = {
    26: ('clay bricks', hexgrid(stone_pattern('cea5a5', '9c2929', '6b2121', '211010')), 9),
    27: ('door closed', hexgrid(door_closed), 0),
    28: ('door open',   hexgrid(door_open), 0),
}

# colours added to free slots of background palette rows
BG_ADD = {9: ['cea5a5', '9c2929', '6b2121', '211010']}

# Colours that must sit at a fixed index. Water draws its shore tiles (24-35)
# with indices 3 (water), 12 (rim) and 13 (bank): row 8 (dirt, which only uses
# 1-2) gets an ice shore there, used by liquid_draw next to ice.
BG_SET = {8: {3: '10108c', 12: '9494ad', 13: 'c6c6de'}}

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
