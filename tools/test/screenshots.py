"""Take the README screenshots (docs/screenshots/*.png) with the debug ROM.

mGBA's GDB stub only exposes memory, so each shot dumps VRAM, palettes, OAM
and the display registers, and render() composes the frame like the GBA
does: backgrounds and sprites by priority, the OBJ window that cuts the
light layer, and alpha blending (night).

usage: python tools/test/screenshots.py   (build the debug ROM first)
"""
import os, shutil, struct, sys, tempfile
from PIL import Image
sys.path.insert(0, os.path.dirname(__file__))
import smoke

smoke.ELF = smoke.ELF.replace('edition.elf', 'edition-debug.elf')
smoke.ROM = smoke.ROM.replace('edition.gba', 'edition-debug.gba')
OUT = os.path.join(smoke.ROOT, 'docs', 'screenshots')

SPRITE_SIZES = {  # (shape, size) -> (w, h)
    (0, 0): (8, 8), (0, 1): (16, 16), (0, 2): (32, 32), (0, 3): (64, 64),
    (1, 0): (16, 8), (1, 1): (32, 8), (1, 2): (32, 16), (1, 3): (64, 32),
    (2, 0): (8, 16), (2, 1): (8, 32), (2, 2): (16, 32), (2, 3): (32, 64),
}

def render(prefix, scroll):
    vram = open(prefix + 'vram.bin', 'rb').read()
    pal = open(prefix + 'pal.bin', 'rb').read()
    oam = open(prefix + 'oam.bin', 'rb').read()
    io = open(prefix + 'io.bin', 'rb').read()
    reg = lambda a: io[a] | io[a + 1] << 8
    color = lambda i: (lambda v: (v & 31, v >> 5 & 31, v >> 10 & 31))(pal[i * 2] | pal[i * 2 + 1] << 8)

    dispcnt, winout = reg(0x00), reg(0x4A)
    bldcnt, bldalpha = reg(0x50), reg(0x52)

    # backgrounds: (priority, index, pixel function)
    bgs = []
    for n in range(4):
        if not dispcnt >> (8 + n) & 1:
            continue
        cnt = reg(0x08 + n * 2)
        cb, sb = (cnt >> 2 & 3) * 0x4000, (cnt >> 8 & 31) * 0x800
        sx, sy = scroll.get(n, (0, 0))
        def px(x, y, cb=cb, sb=sb, sx=sx, sy=sy):
            x, y = (x + sx) & 255, (y + sy) & 255
            e = vram[sb + ((x >> 3) + (y >> 3) * 32) * 2] | vram[sb + ((x >> 3) + (y >> 3) * 32) * 2 + 1] << 8
            tx, ty = x & 7, y & 7
            if e >> 10 & 1: tx = 7 - tx
            if e >> 11 & 1: ty = 7 - ty
            b = vram[cb + (e & 0x3ff) * 32 + ty * 4 + tx // 2]
            i = (b >> 4) if tx & 1 else (b & 15)
            return None if i == 0 else color((e >> 12) * 16 + i)
        bgs.append((cnt & 3, n, px))

    # sprites: top pixel per screen position, and the OBJ window mask
    obj = {}
    objwin = set()
    one_d = dispcnt >> 6 & 1
    for k in range(128):
        a0, a1, a2 = struct.unpack_from('<HHH', oam, k * 8)
        if a0 >> 8 & 3 == 2:
            continue
        w, h = SPRITE_SIZES[(a0 >> 14, a1 >> 14)]
        y0, x0 = a0 & 255, a1 & 511
        if y0 >= 160: y0 -= 256
        if x0 >= 240: x0 -= 512
        mode, tile, prio, p = a0 >> 10 & 3, a2 & 1023, a2 >> 10 & 3, a2 >> 12
        for yy in range(h):
            for xx in range(w):
                X, Y = x0 + xx, y0 + yy
                if not (0 <= X < 240 and 0 <= Y < 160):
                    continue
                sx = w - 1 - xx if a1 >> 12 & 1 else xx
                sy = h - 1 - yy if a1 >> 13 & 1 else yy
                t = tile + (sy // 8) * (w // 8 if one_d else 32) + sx // 8
                b = vram[0x10000 + t * 32 + (sy & 7) * 4 + (sx & 7) // 2]
                i = (b >> 4) if sx & 1 else (b & 15)
                if i == 0:
                    continue
                if mode == 2:
                    objwin.add((X, Y))
                elif (X, Y) not in obj:
                    obj[(X, Y)] = (prio, color(256 + p * 16 + i))

    img = Image.new('RGB', (240, 160))
    eva, evb = min(16, bldalpha & 31), min(16, bldalpha >> 8 & 31)
    for y in range(160):
        for x in range(240):
            ctl = 0x3f
            if dispcnt >> 15 & 1:
                ctl = winout >> 8 if (x, y) in objwin else winout & 0xff
            layers = []
            for prio, n, px in bgs:
                if ctl >> n & 1:
                    c = px(x, y)
                    if c:
                        layers.append((prio, n + 1, c, n))
            if (x, y) in obj and ctl >> 4 & 1:
                layers.append((obj[(x, y)][0], 0, obj[(x, y)][1], 4))
            layers.sort()
            layers.append((4, 9, color(0), 5))
            top, under = layers[0], layers[min(1, len(layers) - 1)]
            c = top[2]
            if (bldcnt >> 6 & 3) == 1 and ctl >> 5 & 1 and \
               bldcnt >> top[3] & 1 and bldcnt >> (8 + under[3]) & 1:
                c = tuple(min(31, (a * eva + b * evb) >> 4) for a, b in zip(top[2], under[2]))
            img.putpixel((x, y), tuple(v << 3 | v >> 2 for v in c))
    return img

def shot(name):
    return [f'dump binary memory {name}vram.bin 0x06000000 0x06018000',
            f'dump binary memory {name}pal.bin 0x05000000 0x05000400',
            f'dump binary memory {name}oam.bin 0x07000000 0x07000400',
            f'dump binary memory {name}io.bin 0x04000000 0x04000060',
            smoke.obs(f'{name}scroll', '%d,%d',
                      f'{smoke.mem(SYM["level_x_offset"], "unsigned int")}, '
                      f'{smoke.mem(SYM["level_y_offset"], "unsigned int")}')]

def find_and_go(entity_type, dx):
    """Move the player 'dx' pixels left of the first entity of a type."""
    return ['set $i = 1', 'set $found = 0', 'while $i < 255 && $found == 0',
            f'  if {smoke.mem("$pl + $i * 14")} == {entity_type}',
            '    set $found = $i', '  end', '  set $i = $i + 1', 'end',
            'if $found',
            f'  set *(unsigned short *) ($pl + 2) = *(unsigned short *) ($pl + $found * 14 + 2) - {dx}',
            '  set *(unsigned short *) ($pl + 4) = *(unsigned short *) ($pl + $found * 14 + 4)',
            'end']

def stairs(dx):
    return [f'set *(unsigned short *) ($pl + 2) = (($px + {dx}) << 4) + 8',
            'set *(unsigned short *) ($pl + 4) = ($py << 4) + 8']

def build_scene():
    s = smoke.Script(SYM, smoke.input_breakpoint(SYM))
    T = lambda dx, dy, t: f'set {smoke.mem(f"$lv + $px + ({dx}) + ($py + ({dy})) * 112")} = {t}'
    s.at(1, 'set $px = $xt', 'set $py = $yt')
    house = []
    for dx in range(0, 5):              # stone-brick barn, door on top
        for dy in range(1, 5):
            edge = dx in (0, 4) or dy in (1, 4)
            house.append(T(dx, dy, 25 if edge else 0))
    house += [T(2, 1, 27), T(2, 2, 33)]  # door, torch inside
    house += [T(dx, -3, 24) for dx in range(-4, 0)]   # planks wall
    house += [T(4, -3, 26), T(4, -2, 26)]             # clay pillar
    house += [f'set *(unsigned short *) ($pl + 2) = (($px - 1) << 4) + 8',
              f'set *(unsigned short *) ($pl + 4) = (($py + 1) << 4) + 8']
    s.at(5, *house)
    s.at(70, *shot('day_'))
    s.at(71, f'set {smoke.mem(SYM["day_time"], "unsigned short")} = 35000')
    s.at(100, *shot('night_'))
    s.at(101, f'set {smoke.mem(SYM["day_time"], "unsigned short")} = 1000', *stairs(2))
    s.at(260, *find_and_go(19, 56))     # level 2: skeleton, spider, bloater
    s.at(275, *shot('cave_'))
    s.at(276, *stairs(5))
    # arriving on stairs arms a delay that only resets off the stairs
    s.at(420, *stairs(6))
    s.at(440, *stairs(8))
    s.at(600, *find_and_go(22, 64))     # level 0: magma golem
    s.at(620, *shot('golem_'), 'kill', 'quit')
    return s.render()

SYM = smoke.symbols()

def main():
    work = tempfile.mkdtemp(prefix='mcee-shots-')
    shutil.copy(smoke.ROM, os.path.join(work, 't.gba'))
    out = smoke.run_session('shots', build_scene(), work, timeout=400)
    scrolls = smoke.observations(out)
    os.makedirs(OUT, exist_ok=True)
    for name in ['day', 'night', 'cave', 'golem']:
        key = f'{name}_scroll'
        if key not in scrolls:
            print('missing shot', name)
            continue
        xo, yo = (int(v) for v in scrolls[key].split(','))
        img = render(os.path.join(work, name + '_'),
                     {1: (xo & 15, yo & 15), 2: (xo & 15, yo & 15)})
        img.resize((480, 320), Image.NEAREST).save(os.path.join(OUT, name + '.png'))
        print('wrote', name)
    shutil.rmtree(work, ignore_errors=True)

if __name__ == '__main__':
    main()
