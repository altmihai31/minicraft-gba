"""16x16 mob / furniture / projectile sprites (mobs.png).

Hand-drawn at 16x16 from the owner's reference sheet (27.10.2026), made
original rather than copies of Minecraft's mobs. Grids hold fills only;
'outline' adds the black outline around every silhouette.

Frame order per mob: front (facing down), back (facing up), side (facing
right), side walk A, side walk B. Front and back are drawn mid-stride: the
game mirrors them to animate walking. update_art.py writes this into
res/images/mobs.png and the sprite palette rows 8-11."""

# palette rows (7 colours each + black at index 14)
ROW8 = dict(W='f7f7f7', G='c6c6c6', B='8c5a39', N='4a2918', P='f7b5ad', Q='de8484', R='a54a4a')
ROW9 = dict(W='f7f7f7', G='c6c6c6', T='ad7b5a', D='6b4a31', O='e7a521', R='c62929', S='d6ad94')
ROW10 = dict(W='f7f7f7', G='c6c6c6', M='949494', H='636363', R='c62929', N='3a2118', B='5a3929')
ROW11 = dict(E='2f4a21', F='63943a', L='a5d652', U='a56b5a', X='3a2421', O='de7b29', Y='efc642')
K = '080808'

def outline(rows):
    g = [list(r) for r in rows]
    assert len(g) == 16, len(g)
    for r in g:
        assert len(r) == 16, (''.join(r), len(r))
    add = []
    for y in range(16):
        for x in range(16):
            if g[y][x] != '.':
                continue
            for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                xx, yy = x + dx, y + dy
                if 0 <= xx < 16 and 0 <= yy < 16 and g[yy][xx] not in '.K':
                    add.append((x, y))
                    break
    for x, y in add:
        g[y][x] = 'K'
    return [''.join(r) for r in g]

E16 = '................'

def compose(top, legs):
    """top: rows 0..len(top)-1, legs fill the rest down to 16 rows."""
    rows = list(top) + list(legs)
    rows += [E16] * (16 - len(rows))
    return outline(rows)

# ================================================================ COW
COW_SIDE_TOP = [
 E16,
 "...........G..G.",
 "...........BBBB.",
 "..........BBBBB.",
 ".BBWWBBBBBBBKBW.",
 ".BWWWBBBBBBBBWW.",
 ".BBWBBBBWWBBBQQ.",
 ".BBBBBBBWWWBBB..",
 ".NBBBBBBBWBBBN..",
 ".NNBBBBBBBBBNN..",
 "..NNNNNQNNNNN...",
]
COW_SIDE_LEGS = [
 ["..BB.NN..BB.NN..",
  "..BB.NN..BB.NN..",
  "..GG.GG..GG.GG.."],
 [".BB..NN...BBNN..",
  "BB...NN...BB.NN.",
  "GG....GG...GG.GG"],
 ["...BBNN..BB..NN.",
  "...BB.NN.BB...NN",
  "...GG..GG.GG..GG"],
]
COW_FRONT = outline([
 E16,
 "...G........G...",
 "...GBBBBBBBBG...",
 "....BBWWWWBB....",
 "....BKWWWWKB....",
 "....BBWWWWBB....",
 "....BQQQQQQB....",
 "...BBQKQQKQBB...",
 "..BBWBQQQQBBWB..",
 "..BWWBBBBBBBWB..",
 "..BBBBBBBBBBBB..",
 "...NNNNNNNNNN...",
 "...BB......BB...",
 "...BB......GG...",
 "...GG...........",
 E16])
COW_BACK = outline([
 E16,
 "...G........G...",
 "....BBBBBBBB....",
 "...BBBBBBBBBB...",
 "..BBBWWBBBBBBB..",
 "..BWWWWBBBBBWB..",
 "..BBWWBBBBBWWB..",
 "..BBBBBBNBBBBB..",
 "..BBBBBBNBBBBB..",
 "..NBBBBBNBBBBN..",
 "..NNNNNQQNNNNN..",
 "...BB......BB...",
 "...BB......GG...",
 "...GG...........",
 E16, E16])
COW = [COW_FRONT, COW_BACK] + [compose(COW_SIDE_TOP, l) for l in COW_SIDE_LEGS]

# ================================================================ PIG
PIG_SIDE_TOP = [
 E16, E16, E16,
 "..........RR....",
 "..PPPPPPPPPPPP..",
 ".PPPPPPPPPPPKPP.",
 ".PPPPPPPPPPPPQQ.",
 ".PPQQPPPQQPPPRQ.",
 "QPPPPPPPPPPPPP..",
 ".QQQQQQQQQQQQ...",
 "..RRRRRRRRRRR...",
]
PIG_SIDE_LEGS = [
 ["..PP.RR...PP.RR.",
  "..PP.RR...PP.RR."],
 [".PP..RR....PPRR.",
  "PP...RR....PP.RR"],
 ["...PPRR..PP...RR",
  "...PP.RR.PP...RR"],
]
PIG_FRONT = outline([
 E16, E16,
 "...RR......RR...",
 "...RPPPPPPPPR...",
 "...PPPPPPPPPP...",
 "...PKPPPPPPKP...",
 "...PPPQQQQPPP...",
 "...PPQRQQRQPP...",
 "..PPPPQQQQPPPP..",
 "..PQPPPPPPPPQP..",
 "..PPPPPPPPPPPP..",
 "..QQQQQQQQQQQQ..",
 "...PP......PP...",
 "...PP......RR...",
 "...RR...........",
 E16])
PIG_BACK = outline([
 E16, E16,
 "...RR......RR...",
 "...RPPPPPPPPR...",
 "..PPPPPPPPPPPP..",
 "..PPPPPPPPPPPP..",
 "..PPPPPPQPPPPP..",
 "..PPPPPQPQPPPP..",
 "..PPPPPPQPPPPP..",
 "..PPPPPPPPPPPP..",
 "..QQQQQRRQQQQQ..",
 "...PP......PP...",
 "...PP......RR...",
 "...RR...........",
 E16, E16])
PIG = [PIG_FRONT, PIG_BACK] + [compose(PIG_SIDE_TOP, l) for l in PIG_SIDE_LEGS]

# ================================================================ CHICKEN
CHICKEN_SIDE_TOP = [
 E16,
 "..........R.....",
 ".........WWW....",
 ".........WKWOO..",
 ".........WWWOO..",
 "........WWWRR...",
 "..W.....WWWR....",
 ".WWWWWWWWWW.....",
 ".WWGGGWWWWW.....",
 ".WWWGGGWWWW.....",
 "..WWWWWWWG......",
 "...GGGGGG.......",
]
CHICKEN_SIDE_LEGS = [
 [".....O..O.......",
  "....OO.OO......."],
 ["....O....O......",
  "...OO....OO....."],
 [".....O.O........",
  ".....OOOO......."],
]
CHICKEN_FRONT = outline([
 E16,
 ".......R........",
 "......RWR.......",
 "......WWW.......",
 ".....WKWKW......",
 ".....WOOOW......",
 ".....WWRWW......",
 "....WWWRWWW.....",
 "...GWWWWWWWG....",
 "...GWWWWWWWG....",
 "...GWWWWWWWG....",
 "....WWWWWWW.....",
 ".....GGGGG......",
 "......O..O......",
 ".....OO..O......",
 E16])
CHICKEN_BACK = outline([
 E16,
 ".......R........",
 "......RWR.......",
 "......WWW.......",
 ".....WWWWW......",
 ".....WWWWW......",
 "....WWWWWWW.....",
 "...GWWWWWWWG....",
 "...GWWWWWWWG....",
 "...GWWWWWWWG....",
 "....WWWGWWW.....",
 ".....WGGGW......",
 "......O..O......",
 "......O..OO.....",
 ".....OO.........",
 E16])
CHICKEN = [CHICKEN_FRONT, CHICKEN_BACK] + [compose(CHICKEN_SIDE_TOP, l) for l in CHICKEN_SIDE_LEGS]

# ================================================================ SHEEP
def sheep_side_top(wool):
    body, shade = ('W', 'G') if wool else ('S', 'T')
    rows = [
     E16, E16,
     "..bb.bbb.bb.....",
     ".bbbbbbbbbbbTT..",
     ".bbbbsbbbbbTTTT.",
     ".bbsbbbbsbbTKTT.",
     ".bbbbbbbbbbbTTS.",
     ".bbbbbsbbbbbTT..",
     ".sbbbbbbbbbsb...",
     "..ssssssssss....",
    ]
    if not wool:
        rows[2] = E16
        rows[3] = "...........TT..."
        rows[4] = "..bbbbbbbbbTTTT."
    return [r.replace('b', body).replace('s', shade) for r in rows]

SHEEP_LEGS = [
 ["..TT.DD...TT.DD.",
  "..TT.DD...TT.DD.",
  "..DD.DD...DD.DD."],
 [".TT..DD....TTDD.",
  "TT...DD....TT.DD",
  "DD....DD....DD.D"],
 ["...TTDD..TT...DD",
  "...TT.DD.TT...DD",
  "...DD..DD.DD..DD"],
]

def sheep_front(wool):
    body, shade = ('W', 'G') if wool else ('S', 'T')
    rows = [
     E16,
     "....bb.bb.bb....",
     "...bbbbbbbbbb...",
     "..bbbTTTTTTbbb..",
     "..bbTKTTTTKTbb..",
     "..bbTTTTTTTTbb..",
     "..bbbTTSSTTbbb..",
     "..bsbbTTTTbbsb..",
     "..bbbbbbbbbbbb..",
     "..bbbbbbbbbbbb..",
     "..ssssssssssss..",
     "...TT......TT...",
     "...TT......DD...",
     "...DD...........",
     E16, E16]
    if not wool:
        rows[1] = E16
        rows[2] = "....b......b...."
    return outline([r.replace('b', body).replace('s', shade) for r in rows])

def sheep_back(wool):
    body, shade = ('W', 'G') if wool else ('S', 'T')
    rows = [
     E16,
     "....bb.bb.bb....",
     "...bbbbbbbbbb...",
     "..bbbbbbbbbbbb..",
     "..bbsbbbbbbsbb..",
     "..bbbbbbbbbbbb..",
     "..bbbbbsbbbbbb..",
     "..bbbbbbbbbsbb..",
     "..bbsbbbbbbbbb..",
     "..bbbbbbbbbbbb..",
     "..ssssssssssss..",
     "...TT......TT...",
     "...TT......DD...",
     "...DD...........",
     E16, E16]
    if not wool:
        rows[1] = E16
        rows[2] = "....bbbbbbbb...."
    return outline([r.replace('b', body).replace('s', shade) for r in rows])

def sheep(wool):
    return [sheep_front(wool), sheep_back(wool)] + \
           [compose(sheep_side_top(wool), l) for l in SHEEP_LEGS]

SHEEP = sheep(True)
SHEARED = sheep(False)

# ================================================================ WOLF
WOLF_SIDE_TOP = [
 E16,
 "..........M..M..",
 "..........GGGGG.",
 "..........GGKGG.",
 ".M.GGGGGGGRRGWW.",
 "MMGGGGGGGGRRGWWK",
 ".GGGGGGGGGGWW...",
 ".GGGMMMGGGGWW...",
 ".MGGGGGGGGGGM...",
 "..MMMMMMMMMMM...",
]
WOLF_SIDE_LEGS = [
 ["..GG.MM...GG.MM.",
  "..GG.MM...GG.MM.",
  "..GG.MM...GG.MM."],
 [".GG..MM....GGMM.",
  "GG...MM....GG.MM",
  "GG....MM....GG.M"],
 ["...GGMM..GG...MM",
  "...GG.MM.GG...MM",
  "...GG..MMGG...MM"],
]
WOLF_FRONT = outline([
 E16,
 "...M.......M....",
 "...MG.....GM....",
 "...GGGGGGGGG....",
 "...GKGGGGGKG....",
 "...GGGWWWGGG....",
 "....GWWKWWG.....",
 "....GRRRRRG.....",
 "...GGWWWWWGG....",
 "...GGWWWWWGG....",
 "...MGGGGGGGM....",
 "....GG...GG.....",
 "....GG...MM.....",
 "....MM..........",
 E16, E16])
WOLF_BACK = outline([
 E16,
 "...M.......M....",
 "...MG.....GM....",
 "...GGGGGGGGG....",
 "...GGGGGGGGG....",
 "....GGGGGGG.....",
 "....RRRRRRR.....",
 "...GGGGGGGGG....",
 "...GGGGMGGGG....",
 "...GGGGMGGGG....",
 "...MGGGMGGGM....",
 "....GG.M.GG.....",
 "....GG...MM.....",
 "....MM..........",
 E16, E16])
WOLF = [WOLF_FRONT, WOLF_BACK] + [compose(WOLF_SIDE_TOP, l) for l in WOLF_SIDE_LEGS]

# ================================================================ SKELETON (with a bow)
SKELETON_FRONT = outline([
 E16,
 ".....WWWWWW.....",
 "....WWWWWWWW....",
 "....WHHWWHHG....",
 "....WHHWWHHG....",
 "....WWWHHWWG....",
 ".....WHWHWG.....",
 "......GWWG......",
 "...WGWWWWWWGW...",
 "...W.WHWWHW.W...",
 "...W.WWWWWW.WB..",
 "...G.WHWWHW.GB..",
 "......WG.WG..B..",
 "......WG.WG.....",
 "......GG..G.....",
 E16])
SKELETON_BACK = outline([
 E16,
 ".....WWWWWW.....",
 "....WWWWWWWW....",
 "....WWWWWWWG....",
 "....WWWWWWWG....",
 "....WWWWWWWG....",
 ".....GWWWWG.....",
 "......GWWG......",
 "...WGWWHWWWGW...",
 "...W.WWHWWW.W...",
 "..BW.WWHWWW.W...",
 "..BG.WWHWWW.G...",
 "..B...WG.WG.....",
 "......WG.WG.....",
 "......G..GG.....",
 E16])
SKELETON_SIDE_TOP = [
 E16,
 ".....WWWWW......",
 "....WWWWWWW.....",
 "....WWWWHHW.....",
 "....WWWWHHW.....",
 "....GWWWWWW.....",
 ".....GWHWHW..B..",
 "......GWWG....B.",
 ".....WWWWWWWWBB.",
 "......WHWW....B.",
 "......WWWW....B.",
 "......WHWW...B..",
]
SKELETON_SIDE_LEGS = [
 ["......WG........",
  "......WG........",
  "......GG........"],
 [".....WG.WG......",
  "....WG...WG.....",
  "....GG...GG....."],
 ["......WGWG......",
  ".....WG..WG.....",
  ".....GG...GG...."],
]
SKELETON = [SKELETON_FRONT, SKELETON_BACK] + \
           [compose(SKELETON_SIDE_TOP, l) for l in SKELETON_SIDE_LEGS]

# ================================================================ SPIDER
SPIDER_FRONT = outline([
 E16, E16, E16,
 "......BBBB......",
 "..N..BBBBBB..N..",
 ".N.N.BBBBBB.N.N.",
 "N...NBBBBBBN...N",
 "..NNNBRBBRBNNN..",
 ".N..NBBRRBBN..N.",
 "N..N.NBBBBN.N..N",
 "..N...NNNN...N..",
 ".N............N.",
 E16, E16, E16, E16])
SPIDER_BACK = outline([
 E16, E16, E16,
 "......BBBB......",
 "..N..BBBBBB..N..",
 ".N.N.BBNNBB.N.N.",
 "N...NBBNNBBN...N",
 "..NNNBBBBBBNNN..",
 ".N..NBBNNBBN..N.",
 "N..N.NBBBBN.N..N",
 "..N...NNNN...N..",
 ".N............N.",
 E16, E16, E16, E16])
SPIDER_SIDE_TOP = [
 E16, E16, E16, E16,
 ".....BBBB.......",
 "...BBBBBBBB.....",
 "..BBBBBBBBBNN...",
 "..BBNBBBBBBNNRN.",
 "..BBBBBBBBBNNNN.",
 "...BBBBBBBB.....",
]
SPIDER_SIDE_LEGS = [
 ["..N.N.N..N.N.N..",
  ".N..N..N..N..N..",
  "N..N....N..N..N."],
 ["..N.N..N.N..N...",
  ".N..N.N..N..N...",
  ".N.N..N..N...N.."],
 ["...N.N.N..N.N.N.",
  "..N..N..N..N..N.",
  "..N..N...N..N..N"],
]
SPIDER = [SPIDER_FRONT, SPIDER_BACK] + [compose(SPIDER_SIDE_TOP, l) for l in SPIDER_SIDE_LEGS]

# ================================================================ BLOATER (bloated moss creature)
BLOATER_FRONT = outline([
 E16, E16,
 ".....FFFFFF.....",
 "....FLLFFFFF....",
 "...FLFFFFFFFF...",
 "...FFXFFFFXFF...",
 "...FFFFFFFFFF...",
 "..FFFXXXXXXFFF..",
 "..FUFXUXXUXFUF..",
 "..FFFFFFFFFFFF..",
 "..EFUUFFFFUUFE..",
 "..EEFFFFFFFFEE..",
 "...EEF....FEE...",
 "...EE.....EE....",
 "...EE...........",
 E16])
BLOATER_BACK = outline([
 E16, E16,
 ".....FFFFFF.....",
 "....FLLFFFFF....",
 "...FLFFUUFFFF...",
 "...FFFUUUFFFF...",
 "...FFFFFFFFFF...",
 "..FFFFFFFFUUFF..",
 "..FUUFFFFUUUFF..",
 "..FFFFFFFFFFFF..",
 "..EFFFFUUFFFFE..",
 "..EEFFFFFFFFEE..",
 "...EEF....FEE...",
 "...EE.....EE....",
 "...EE...........",
 E16])
BLOATER_SIDE_TOP = [
 E16, E16,
 ".....FFFFFF.....",
 "....FLLFFFFFF...",
 "...FLFFFFUFFXF..",
 "...FFFFFFFFFFFF.",
 "..FFUUFFFFFXXXF.",
 "..FFUFFFFFFUXUF.",
 "..EFFFFFFFUUFF..",
 "..EEFFFFFFFFE...",
 "...EEEEEEEEE....",
]
BLOATER_SIDE_LEGS = [
 ["....EE..EE......",
  "....EE..EE......"],
 ["...EE....EE.....",
  "..EE......EE...."],
 [".....EEEE.......",
  ".....EE.EE......"],
]
BLOATER = [BLOATER_FRONT, BLOATER_BACK] + [compose(BLOATER_SIDE_TOP, l) for l in BLOATER_SIDE_LEGS]

# ================================================================ MAGMA GOLEM
GOLEM_FRONT = outline([
 E16,
 ".....XXXXXX.....",
 "....XXXXXXXX....",
 "....XYOXXOYX....",
 "....XXXXXXXX....",
 "....XXOOOOXX....",
 "..XXXXXXXXXXXX..",
 ".XXXOXXXXXXOXXX.",
 ".XOXXXOYYOXXXOX.",
 ".XXX.XOYYOX.XXX.",
 ".XOX.XXOOXX.XOX.",
 ".XXX.XXXXXX.XXX.",
 "......XX.XX.....",
 "......XO.XX.....",
 "......XX........",
 E16])
GOLEM_BACK = outline([
 E16,
 ".....XXXXXX.....",
 "....XXXOXXXX....",
 "....XXXXOXXX....",
 "....XXXXXXXX....",
 "....XXXXXXXX....",
 "..XXXXXXOXXXXX..",
 ".XXXXXXOXXXXXXX.",
 ".XOXXXXOOXXXXOX.",
 ".XXX.XXXOXX.XXX.",
 ".XOX.XXXOXX.XOX.",
 ".XXX.XXXXXX.XXX.",
 "......XX.XX.....",
 "......XX.XO.....",
 ".........XX.....",
 E16])
GOLEM_SIDE_TOP = [
 E16,
 "......XXXXX.....",
 ".....XXXXXXX....",
 ".....XXXXXYO....",
 ".....XXXXXXX....",
 ".....XXXXOOX....",
 "....XXXXXXXXX...",
 "...XXXOXXXXXXX..",
 "...XOXXOYOXXOX..",
 "...XXXOYYOXXXX..",
 "....XXXOOXXOX...",
 "....XXXXXXXXX...",
]
GOLEM_SIDE_LEGS = [
 [".....XX.XX......",
  ".....XO.XX......",
  ".....XX.XX......"],
 ["....XX...XX.....",
  "...XO.....XX....",
  "...XX.....XX...."],
 [".....XXXXX......",
  "......XOXX......",
  "......XXXX......"],
]
GOLEM = [GOLEM_FRONT, GOLEM_BACK] + [compose(GOLEM_SIDE_TOP, l) for l in GOLEM_SIDE_LEGS]

# ================================================================ EXTRAS
ARROW_H = [E16] * 6 + [
 "..W.........K...",
 ".WWDDDDDDDDDGK..",
 "..W.........K...",
] + [E16] * 7
# points up: rotate the horizontal arrow 90 degrees counter-clockwise
ARROW_V = [''.join(ARROW_H[x][15 - y] for x in range(16)) for y in range(16)]
XP_ORB = outline([E16] * 5 + [
 "......FFF.......",
 ".....FLLFF......",
 ".....FLFFF......",
 ".....FFFFE......",
 "......FEE.......",
] + [E16] * 6)

ROW6 = dict(a='211010', b='846b52', c='ad9463', D='393939', M='636363', z='313118', u='8c8c63',
            p='e7e7a5', W='f7f7f7', R='9c2929', g='4ac64a')
BED = outline([
 E16, E16,
 "...cccccccccc...",
 "...bbbbbbbbbb...",
 "...WWWWWWWWWW...",
 "...WpWWWWWWpW...",
 "...RRRRRRRRRR...",
 "...RRRRRRRRRR...",
 "...RRaRRRRRRR...",
 "...RRRRRRaRRR...",
 "...RRRRRRRRRR...",
 "...WWWWWWWWWW...",
 "...bbbbbbbbbb...",
 "...b........b...",
 E16, E16])
ENCHANT_TABLE = outline([
 E16, E16,
 ".....WWW.WWW....",
 "....pWWWaWWWp...",
 "..RRRRRRRRRRRR..",
 "..RRRRRRRRRRRR..",
 "..gzzzzzzzzzzg..",
 "..zDDDDDDDDDDz..",
 "..zDgDDDDDDgDz..",
 "..zDDDDDDDDDDz..",
 "..zzzzzzzzzzzz..",
 "..gzzzzzzzzzzg..",
 E16, E16, E16, E16])

EXTRAS = [(ARROW_H, ROW9), (ARROW_V, ROW9), (XP_ORB, ROW11), (BED, ROW6), (ENCHANT_TABLE, ROW6)]


# ================================================================ sheet layout
def with_black(legend):
    return dict(legend, K=K)

MOBS = [  # (name, frames, legend), one mobs.png row each
 ('cow', COW, with_black(ROW8)), ('pig', PIG, with_black(ROW8)),
 ('chicken', CHICKEN, with_black(ROW9)), ('sheep', SHEEP, with_black(ROW9)),
 ('sheep (sheared)', SHEARED, with_black(ROW9)), ('wolf', WOLF, with_black(ROW10)),
 ('skeleton', SKELETON, with_black(ROW10)), ('spider', SPIDER, with_black(ROW10)),
 ('bloater', BLOATER, with_black(ROW11)), ('magma golem', GOLEM, with_black(ROW11)),
]
EXTRAS = [('arrow (horizontal)', ARROW_H, with_black(ROW9)),
          ('arrow (vertical)', ARROW_V, with_black(ROW9)),
          ('XP orb', XP_ORB, with_black(ROW11)),
          ('bed', BED, with_black(ROW6)),
          ('enchanting table', ENCHANT_TABLE, with_black(ROW6))]

# Sprite palette rows 8-11 belong to the mod mobs only. Indices 1-4 and 8-10
# hold the 7 body colours and 14 is black: the same layout as the white
# "hurt" row 5, so the damage flash keeps working.
BLUES = ['10108c', '42429c', '9c9cc6']
def prow(legend):
    c = list(legend.values())
    assert len(c) == 7, c
    return [None] + c[:4] + BLUES + c[4:] + [None, None, None, K, None]

SPRITE_ROWS = {8: prow(ROW8), 9: prow(ROW9), 10: prow(ROW10), 11: prow(ROW11)}
