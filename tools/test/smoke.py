"""Smoke test for Minicraft Expanded Edition, run in mGBA through its GDB stub.

Plays the normal ROM without touching the keyboard: GDB stops at the end of
input_update every frame and writes the pressed keys, sets up situations
directly in memory, and prints observations that this script checks.

Sessions (each starts mGBA on a copy of the ROM in a temporary folder):
  play  new game: block placement rules and messages, enchanting table,
        night light layer, bed (skip night + autosave), split resource stacks
  save  new game, levels 1-3 made incompressible (~90 KB save), saved from
        the pause menu: file must be 128 KB with a valid checksum
  load  boots that save: start screen must accept it, levels 1-3 must match

usage: python tools/test/smoke.py [--keep]
env:   MGBA (mGBA executable), GDB (default arm-none-eabi-gdb)
Close any mGBA instance using GDB port 2345 first.
"""
import os, re, shutil, struct, subprocess, sys, tempfile, time

ROOT = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..'))
ELF = os.path.join(ROOT, 'bin', 'minicraft-expanded-edition.elf')
ROM = os.path.join(ROOT, 'bin', 'minicraft-expanded-edition.gba')
GDB = os.environ.get('GDB', 'arm-none-eabi-gdb')
MGBA = os.environ.get('MGBA') or next(
    (p for p in [r'C:\Program Files\mGBA\mGBA.exe', shutil.which('mgba-qt'),
                 shutil.which('mgba')] if p and os.path.exists(p)), None)

# game constants (include/*.h)
LEVEL_W = 112
LEVEL_SIZE = LEVEL_W * LEVEL_W
LEVEL_BYTES = 2 * LEVEL_SIZE + 255 * 14
ENTITIES = 2 * LEVEL_SIZE
GRASS, ROCK, FLOWER, FARMLAND, WHEAT = 0, 1, 3, 11, 12
PLANKS_TILE, TORCH_TILE = 24, 33
PLANKS, TORCH, SWORD, TABLE, BED, WORKBENCH = 33, 39, 28, 72, 66, 21
ITEM_ENTITY, XP_ORB_ENTITY = 10, 24
IRON_ORE, PICK, GOLD_ARMOR = 18, 30, 70
BG3_MAP = 0x06009800  # screenblock 19
KEY_A, KEY_B, KEY_START = 1, 2, 8
NIGHT, DAY = 35000, 1000  # day_time values (src/survival.c)

def symbols():
    out = subprocess.run(['arm-none-eabi-nm', ELF], capture_output=True,
                         text=True, check=True).stdout
    return {l.split()[2]: int(l.split()[0], 16) for l in out.splitlines()
            if len(l.split()) == 3}

def input_breakpoint(sym):
    # the instruction after 'mvns r1, r1': r1 = keys held this frame
    start = sym['input_update'] & ~1
    out = subprocess.run(['arm-none-eabi-objdump', '-d', ELF,
                          f'--start-address={start:#x}',
                          f'--stop-address={start + 32:#x}'],
                         capture_output=True, text=True, check=True).stdout
    m = re.search(r'^\s*([0-9a-f]+):\s+[0-9a-f]+\s+mvns\s+r1, r1', out, re.M)
    return int(m.group(1), 16) + 2

# ------------------------------------------------------------ gdb script
class Script:
    """Builds a gdb script: 'steps' run on given frames after the game starts."""
    def __init__(self, sym, bp):
        self.sym, self.bp = sym, bp
        self.steps = {}

    def at(self, frame, *cmds):
        self.steps.setdefault(frame, []).extend(cmds)

    def render(self, menu=None):
        """'menu': commands run every frame before the game starts (default:
        tap A + Start through the title screens into a new game)."""
        s = self.sym
        menu = menu or ['if ($f % 40) < 2', f'  set $r1 = {KEY_A | KEY_START}', 'end']
        lines = [
            'set pagination off', 'set confirm off',
            'file ' + ELF.replace(os.sep, '/'),
            'target remote 127.0.0.1:2345',
            'set $f = 0', 'set $g = 0', 'set $s = 0', 'set $started = 0',
            f'break *{self.bp:#x}', 'commands', 'silent',
            'set $f = $f + 1', 'set $r1 = 0',
            'if $started == 0',
            f'  if *(unsigned int *) {s["scene"]:#x} == {s["scene_game"]:#x}',
            '    set $started = 1',
            '  else',
            *['    ' + c for c in menu],
            '  end',
            'else',
            '  set $g = $g + 1',
            f'  set $lv = {s["levels"]:#x} + *(unsigned char *) {s["current_level"]:#x} * {LEVEL_BYTES}',
            f'  set $pl = $lv + {ENTITIES}',
            '  set $xt = *(unsigned short *) ($pl + 2) >> 4',
            '  set $yt = *(unsigned short *) ($pl + 4) >> 4',
        ]
        for frame in sorted(self.steps):
            lines.append(f'  if $g == {frame}')
            lines += ['    ' + c for c in self.steps[frame]]
            lines.append('  end')
        lines += ['end', 'continue', 'end', 'continue']
        return '\n'.join(lines) + '\n'

def run_session(name, script, workdir, timeout=240):
    path = os.path.join(workdir, f'{name}.gdb')
    with open(path, 'w') as f:
        f.write(script)
    rom = os.path.join(workdir, 't.gba')
    kw = {}
    if os.name == 'nt':
        si = subprocess.STARTUPINFO()
        si.dwFlags |= subprocess.STARTF_USESHOWWINDOW
        si.wShowWindow = 7  # minimized, not activated
        kw['startupinfo'] = si
    emu = subprocess.Popen([MGBA, '-g', rom], **kw)
    try:
        time.sleep(4)
        res = subprocess.run([GDB, '-q', '-batch', '-x', path], cwd=workdir,
                             capture_output=True, text=True, timeout=timeout)
        time.sleep(1)
    finally:
        emu.kill()
        emu.wait()
    out = res.stdout + res.stderr
    with open(os.path.join(workdir, f'{name}.log'), 'w') as f:
        f.write(out)
    return out

# ------------------------------------------------------------ helpers
def mem(addr, t='unsigned char'):
    return f'*({t} *) ({addr})'

def obs(key, fmt, expr):
    return f'printf "OBS {key}={fmt}\\n", {expr}'

def face(dir_):
    """Commands that set the player's direction and $fx/$fy (tile in front)."""
    dx = (dir_ == 3) - (dir_ == 1)
    dy = (dir_ == 2) - (dir_ == 0)
    return [f'set {mem("$pl + 8")} = {dir_}',
            f'set $fx = (*(unsigned short *) ($pl + 2) + {dx * 12}) >> 4',
            f'set $fy = (*(unsigned short *) ($pl + 4) - 2 + {dy * 12}) >> 4']

TILE = mem('$lv + $fx + $fy * 112')

def count_items():
    return ['set $items = 0', 'set $i = 1', 'while $i < 255',
            f'  if {mem("$pl + $i * 14")} == {ITEM_ENTITY}',
            '    set $items = $items + 1', '  end',
            '  set $i = $i + 1', 'end']

# ------------------------------------------------------------ sessions
def play_session(sym, bp):
    s = Script(sym, bp)
    act, msg = sym['player_active_item'], sym['message']
    xp, day = sym['player_xp'], sym['day_time']
    message = obs('{k}', '%s', f'*(char **) {msg:#x}')

    def item(type_, count=5):
        return [f'set {mem(act)} = {type_}',
                f'set {mem(act + 2, "unsigned short")} = {count}']

    # clear an 11x11 grass area around the player
    s.at(60, 'set $y = $yt - 5', 'while $y <= $yt + 5', '  set $x = $xt - 5',
         '  while $x <= $xt + 5',
         f'    set {mem("$lv + $x + $y * 112")} = 0',
         f'    set {mem("$lv + 12544 + $x + $y * 112")} = 0',
         '    set $x = $x + 1', '  end', '  set $y = $y + 1', 'end')

    # building blocks: grass, flower, farmland, wheat; torch on a flower
    frame = 80
    for name, under, itm in [('planks_grass', GRASS, PLANKS),
                             ('planks_flower', FLOWER, PLANKS),
                             ('planks_farmland', FARMLAND, PLANKS),
                             ('planks_wheat', WHEAT, PLANKS),
                             ('torch_flower', FLOWER, TORCH)]:
        s.at(frame, *face(1), f'set {TILE} = {under}',
             f'set {mem("$lv + 12544 + $fx + $fy * 112")} = 50',
             *item(itm), *count_items(), 'set $before = $items', 'set $r1 = 1')
        s.at(frame + 15, *face(1), *count_items(),
             obs(f'{name}.tile', '%d', TILE),
             obs(f'{name}.count', '%d', mem(act + 2, 'unsigned short')),
             obs(f'{name}.drops', '%d', '$items - $before'))
        frame += 30

    # not placeable on rock
    s.at(frame, *face(1), f'set {TILE} = {ROCK}', *item(PLANKS), 'set $r1 = 1')
    s.at(frame + 15, message.format(k='rock.message'), *face(1),
         obs('rock.tile', '%d', TILE))
    frame += 30

    # enchanting table to the right, then B with different items / XP
    s.at(frame, *face(3), f'set {TILE} = {GRASS}', *item(TABLE, 1), 'set $r1 = 1')
    frame += 30
    for name, itm, lvl, have_xp in [('ench_planks', PLANKS, 0, 100),
                                    ('ench_noxp', SWORD, 0, 0),
                                    ('ench_1', SWORD, 0, 40),
                                    ('ench_noxp2', SWORD, 1, 10),
                                    ('ench_2', SWORD, 1, 100),
                                    ('ench_max', SWORD, 2, 100)]:
        s.at(frame, *face(3), f'set {mem(act)} = {itm}',
             f'set {mem(act + 2)} = 0', f'set {mem(act + 3)} = {lvl}',
             f'set {mem(xp, "unsigned short")} = {have_xp}', 'set $r1 = 2')
        s.at(frame + 10, message.format(k=f'{name}.message'),
             obs(f'{name}.xp', '%d', mem(xp, 'unsigned short')),
             obs(f'{name}.level', '%d', mem(act + 3)),
             # status bar: "WOOD+SWRD" from column 21 of row 18
             obs(f'{name}.mark', '%c', f'{mem(BG3_MAP + (18 * 32 + 25) * 2, "unsigned short")} & 0xff'))
        frame += 25

    # a solid block can't go where the table stands
    s.at(frame, *face(3), *item(PLANKS), 'set $r1 = 1')
    s.at(frame + 15, message.format(k='blocked.message'), *face(3),
         obs('blocked.tile', '%d', TILE))
    frame += 30

    # breaking iron ore drops XP orbs
    s.at(frame, *face(1), f'set {TILE} = {IRON_ORE}',
         f'set {mem("$lv + 12544 + $fx + $fy * 112")} = 30',
         f'set {mem(act)} = {PICK}', f'set {mem(act + 2)} = 4', f'set {mem(act + 3)} = 0',
         'set $r1 = 1')
    s.at(frame + 3, *face(1), obs('ore.tile', '%d', TILE), 'set $orbs = 0', 'set $i = 1',
         'while $i < 255', f'  if {mem("$pl + $i * 14")} == {XP_ORB_ENTITY}',
         '    set $orbs = $orbs + 1', '  end', '  set $i = $i + 1', 'end',
         obs('ore.orbs', '%d', '$orbs'))
    frame += 20

    # the armor pip takes the worn armor's colour (gold)
    s.at(frame, f'set {mem(sym["player_armor"])} = {GOLD_ARMOR}')
    s.at(frame + 3, obs('armor.color', '%d', mem(0x05000000 + (10 * 16 + 5) * 2, 'unsigned short')))
    s.at(frame + 4, f'set {mem(sym["player_armor"])} = 255')
    frame += 10

    # night: light layer on and blended; day: off
    bldcnt = mem(0x04000050, 'unsigned short')
    s.at(frame, f'set {mem(day, "unsigned short")} = {NIGHT}')
    s.at(frame + 5, obs('night.light', '%d', mem(sym['light_layer_on'])),
         obs('night.bldcnt', '%d', bldcnt))
    s.at(frame + 6, f'set {mem(day, "unsigned short")} = {DAY}')
    s.at(frame + 11, obs('day.light', '%d', mem(sym['light_layer_on'])),
         obs('day.bldcnt', '%d', bldcnt))
    frame += 20

    # bed above the player: B at night sleeps until morning and saves
    s.at(frame, *face(0), f'set {TILE} = {GRASS}', *item(BED, 1), 'set $r1 = 1')
    s.at(frame + 20, *face(0), f'set {mem(day, "unsigned short")} = {NIGHT}',
         *item(PLANKS), 'set $r1 = 2')
    s.at(frame + 30, message.format(k='bed.message'),
         obs('bed.day_time', '%d', mem(day, 'unsigned short')))
    frame += 40

    # wood split over two slots (3 + 3): the workbench must count 6, so the
    # first craftable recipe is the wood sword (5 wood), not planks (1)
    inv = sym['player_inventory']
    s.at(frame, *face(2), f'set {TILE} = {GRASS}', *item(WORKBENCH, 1), 'set $r1 = 1')
    s.at(frame + 20, *face(2), f'set {mem(inv)} = 2',
         f'set {mem(inv + 2)} = 0', f'set {mem(inv + 4, "unsigned short")} = 3',
         f'set {mem(inv + 6)} = 0', f'set {mem(inv + 8, "unsigned short")} = 3',
         f'set {mem(act)} = 255', 'set $r1 = 2')
    s.at(frame + 35, obs('split.menu', '%d',
                         f'{mem(sym["scene"], "unsigned int")} == {sym["scene_crafting"]:#x}'),
         'set $r1 = 1')
    s.at(frame + 45, 'set $wood = 0', 'set $sword = 0', 'set $i = 0',
         f'while $i < {mem(inv)}',
         f'  if {mem(f"{inv + 2} + $i * 4")} == 0',
         f'    set $wood = $wood + {mem(f"{inv + 4} + $i * 4", "unsigned short")}',
         '  end',
         f'  if {mem(f"{inv + 2} + $i * 4")} == {SWORD}',
         '    set $sword = 1',
         '  end',
         '  set $i = $i + 1', 'end',
         obs('split.wood_left', '%d', '$wood'), obs('split.sword', '%d', '$sword'))
    s.at(frame + 50, obs('done', '%d', 1), 'kill', 'quit')
    return s.render()

EXPECT_PLAY = {
    'planks_grass.tile': PLANKS_TILE, 'planks_grass.count': 4, 'planks_grass.drops': 0,
    'planks_flower.tile': PLANKS_TILE, 'planks_flower.count': 4,
    'planks_farmland.tile': PLANKS_TILE, 'planks_farmland.count': 4,
    'planks_wheat.tile': PLANKS_TILE, 'planks_wheat.count': 4,
    'torch_flower.tile': TORCH_TILE, 'torch_flower.count': 4,
    'rock.message': 'CANT PLACE', 'rock.tile': ROCK,
    'ench_planks.message': 'NEED TOOL',
    'ench_noxp.message': 'NEED 30 XP', 'ench_noxp.level': 0,
    'ench_1.message': 'ENCHANTED!', 'ench_1.xp': 10, 'ench_1.level': 1, 'ench_1.mark': '+',
    'ench_noxp2.message': 'NEED 60 XP', 'ench_noxp2.level': 1,
    'ench_2.message': 'LEVEL II!', 'ench_2.xp': 40, 'ench_2.level': 2, 'ench_2.mark': '*',
    'ore.tile': 5, 'ore.orbs': 1, 'armor.color': 0x379c,
    'ench_max.message': 'MAX LEVEL', 'ench_max.level': 2,
    'blocked.message': 'BLOCKED', 'blocked.tile': GRASS,
    'night.light': 1, 'day.light': 0, 'day.bldcnt': 0,
    'bed.message': 'SAVED', 'bed.day_time': lambda v: int(v) < 100,
    'split.menu': 1, 'split.wood_left': 1, 'split.sword': 1,
    'done': 1,
}
EXPECT_PLAY['planks_flower.drops'] = lambda v: int(v) >= 1
EXPECT_PLAY['planks_wheat.drops'] = lambda v: int(v) >= 1
EXPECT_PLAY['night.bldcnt'] = lambda v: (int(v) >> 6) & 3 == 1  # alpha blend

PATTERN = bytes(i & 1 for i in range(LEVEL_SIZE))

def data_addr(sym, level):
    return sym['levels'] + level * LEVEL_BYTES + LEVEL_SIZE

def save_session(sym, bp, workdir):
    with open(os.path.join(workdir, 'pattern.bin'), 'wb') as f:
        f.write(PATTERN)
    s = Script(sym, bp)
    s.at(30, *[f'restore pattern.bin binary {data_addr(sym, l):#x}' for l in (1, 2, 3)])
    s.at(60, f'set $r1 = {KEY_START}')
    s.at(80, f'set $r1 = {KEY_A}')
    s.at(120, obs('save_failed', '%d', mem(sym['save_failed'])))
    s.at(130, f'set $r1 = {KEY_START}')
    s.at(700, obs('done', '%d', 1), 'kill', 'quit')
    return s.render()

def load_session(sym, bp):
    s = Script(sym, bp)
    s.at(1, *[f'dump binary memory loaded{l}.bin {data_addr(sym, l):#x} '
              f'{data_addr(sym, l) + LEVEL_SIZE:#x}' for l in (1, 2, 3)],
         obs('done', '%d', 1), 'kill', 'quit')
    # tap A through the title, wait on the start screen, then A (LOAD GAME)
    return s.render(menu=[
        f'if *(unsigned int *) {sym["scene"]:#x} == {sym["scene_start"]:#x}',
        '  set $s = $s + 1',
        '  if $s == 30',
        '    ' + obs('can_load', '%d', mem(sym['can_load'])),
        '    ' + obs('checksum', '%d', mem(sym['checksum_verified'])),
        f'    set $r1 = {KEY_A}',
        '  end',
        'else',
        '  if $s == 0 && ($f % 40) < 2',
        f'    set $r1 = {KEY_A}',
        '  end',
        'end'])

def decode_save(d):
    """Level data arrays of a save file (layout in src/storage.c)."""
    o = 64 + 3
    for _ in range(33):           # player inventory + 32 chests
        o += 1 + 3 * d[o]
    o += 14
    for _ in range(5):
        o += 1 + 14 * d[o]
    arrays = []
    for _ in range(10):           # 5 tile arrays, then 5 data arrays
        out = bytearray()
        while len(out) < LEVEL_SIZE:
            out += bytes([d[o]]) * (d[o + 1] + 1)
            o += 2
        arrays.append(bytes(out))
    return arrays[5:], o

# ------------------------------------------------------------ main
def check(results, expect, failures, session):
    for key, want in expect.items():
        got = results.get(key)
        ok = got is not None and (want(got) if callable(want) else got == str(want))
        print(f'{"PASS" if ok else "FAIL"} {session}: {key} = {got}'
              + ('' if ok or callable(want) else f' (want {want})'))
        if not ok:
            failures.append(f'{session}: {key}')

def observations(out):
    return dict(re.findall(r'^OBS ([\w.]+)=(.*)$', out, re.M))

def main():
    if not MGBA:
        sys.exit('mGBA not found: set MGBA to its executable')
    sym = symbols()
    bp = input_breakpoint(sym)
    work = tempfile.mkdtemp(prefix='mcee-smoke-')
    failures = []
    try:
        shutil.copy(ROM, os.path.join(work, 't.gba'))

        out = run_session('play', play_session(sym, bp), work)
        check(observations(out), EXPECT_PLAY, failures, 'play')

        if os.path.exists(os.path.join(work, 't.sav')):
            os.remove(os.path.join(work, 't.sav'))
        out = run_session('save', save_session(sym, bp, work), work)
        check(observations(out), {'save_failed': 0, 'done': 1}, failures, 'save')

        save = open(os.path.join(work, 't.sav'), 'rb').read()
        stored = struct.unpack('<I', save[4:8])[0]
        data, end = decode_save(save)
        file_ok = {
            'size': len(save) == 128 * 1024,
            'game code': save[:4] == b'ZMCE',
            'checksum': stored == sum(save[8:]) & 0xffffffff,
            'over 64 KB': end > 64 * 1024,
            'levels 1-2 saved': data[1] == PATTERN and data[2] == PATTERN,
        }
        for k, ok in file_ok.items():
            print(f'{"PASS" if ok else "FAIL"} save file: {k}'
                  + (f' ({end / 1024:.1f} KB)' if k == 'over 64 KB' else ''))
            if not ok:
                failures.append(f'save file: {k}')

        out = run_session('load', load_session(sym, bp), work)
        check(observations(out), {'can_load': 1, 'checksum': 1, 'done': 1},
              failures, 'load')
        # levels 1 and 2 don't tick while the player is on the surface;
        # level 3 ticked once: damage recovery may lower a few bytes by 1
        for l in (1, 2, 3):
            path = os.path.join(work, f'loaded{l}.bin')
            got = open(path, 'rb').read() if os.path.exists(path) else b''
            diff = [i for i in range(len(got)) if got[i] != data[l][i]]
            ok = len(got) == LEVEL_SIZE and (
                not diff if l < 3 else
                len(diff) < 300 and all(got[i] == data[l][i] - 1 for i in diff))
            print(f'{"PASS" if ok else "FAIL"} load: level {l} data matches the save'
                  + (f' ({len(diff)} bytes recovered one tick of damage)' if diff and ok else ''))
            if not ok:
                failures.append(f'load: level {l}')
    finally:
        if '--keep' in sys.argv:
            print('work folder:', work)
        else:
            shutil.rmtree(work, ignore_errors=True)

    print(f'\n{len(failures)} failure(s)' if failures else '\nall passed')
    sys.exit(1 if failures else 0)

if __name__ == '__main__':
    main()
