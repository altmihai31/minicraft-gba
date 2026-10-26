"""New 16x16 mob / furniture / projectile sprites (mobs.png).
Frame order per mob: front(down), back(up), side(right), side-walk-A, side-walk-B."""

S8 = dict(K='080808', W='f7f7f7', L='c6c6c6', D='393939', Y='e7e76b', P='ce9c84', Q='bd7b7b', R='9c2929')
S9 = dict(K='080808', W='f7f7f7', L='c6c6c6', G='949494', M='636363', P='ce9c84', T='ad9463', D='393939')
S10 = dict(K='080808', A='211010', B='4a3131', R='9c2929', Y='e7e76b', E='215221', F='429c42', H='8cd68c')
S11 = dict(K='080808', D='393939', M='636363', O='de7b29', Y='e7e76b', R='9c2929', T='ad9463', L='c6c6c6')
S6 = dict(a='211010', b='846b52', c='ad9463', D='393939', M='636363', z='313118', u='8c8c63',
          p='e7e7a5', W='f7f7f7', R='9c2929', K='080808', g='4ac64a')

# New sprite palette rows 8-11. Indices 1-4, 8-10 = body colours, 14 = outline:
# matches the white "hurt" row 5 so the damage flash keeps working.
BLUES = ['10108c', '42429c', '9c9cc6']
def prow(c1to4, c8to10):
    return [None] + c1to4 + BLUES + c8to10 + [None, None, None, '080808', None]
SPRITE_ROWS = {
    8:  prow(['f7f7f7', 'c6c6c6', '393939', 'e7e76b'], ['ce9c84', 'bd7b7b', '9c2929']),
    9:  prow(['f7f7f7', 'c6c6c6', '949494', '636363'], ['ce9c84', 'ad9463', '393939']),
    10: prow(['211010', '4a3131', '9c2929', 'e7e76b'], ['215221', '429c42', '8cd68c']),
    11: prow(['393939', '636363', 'de7b29', 'e7e76b'], ['9c2929', 'ad9463', 'c6c6c6']),
}
FURNITURE_ADD = {6: ['9c2929']}   # bed blanket (furniture always uses sprite row 6)

E = '................'

def legs(base, near, far, main, dark, top=12, length=2):
    g = [list(r) for r in base]
    for x in far:
        for y in range(top, top + length): g[y][x] = dark; g[y][x + 1] = dark
        g[top + length][x] = 'K'; g[top + length][x + 1] = 'K'
    for x in near:
        for y in range(top, top + length): g[y][x] = main; g[y][x + 1] = dark
        g[top + length][x] = 'K'; g[top + length][x + 1] = 'K'
    return [''.join(r) for r in g]

def quad(side_body, front, back, main, dark, stand, wa, wb, top=12, length=2):
    return [front, back,
            legs(side_body, stand, [], main, dark, top, length),
            legs(side_body, wa[0], wa[1], main, dark, top, length),
            legs(side_body, wb[0], wb[1], main, dark, top, length)]

QL = ["...{0}.{0}.{0}.{0}..", "...{0}.{0}.KK.{0}..", "...KK.KK....KK.."]
def qlegs(pair): return [r.format(pair) for r in QL]

# ---------------------------------------------------------------- COW
cow_side = [E, E,
 "..........KL..LK",
 "..........KWWWWK",
 ".KKKKKKKKKKWWKWK",
 "KKWWWDDWWWKWWWWK",
 "DKWDDDWWWWKWPPPK",
 "DKWDDWWWLLKKPQPK",
 "DKWWWWWDDLLKKKK.",
 ".KLWWWDDDLLK....",
 ".KLLLLLLLLLK....",
 ".KKKKKKKKKKK....", E, E, E, E]
cow_front = [E,
 "....LK....KL....",
 "....KKKKKKKK....",
 "...KWWWWWWWWK...",
 "...KWKWWWWKWK...",
 "...KWWWWWWWWK...",
 "..KDKWPPPPWKDK..",
 "..KWKPQPPQPKWK..",
 "..KWDKPPPPKWDK..",
 "..KWDDKKKKWWWK..",
 "..KWWWWDDWWWWK..",
 "..KLLLWWWWLLLK..",
 "..KKKKKKKKKKKK.."] + qlegs('WL')
cow_back = [E,
 "....LK....KL....",
 "....KKKKKKKK....",
 "...KWWWWWWWWK...",
 "...KWWDDWWWWK...",
 "..KKWWWWWWWWKK..",
 "..KWKKKKKKKKWK..",
 "..KWDDWWWWWWWK..",
 "..KWDDWWWDWDDK..",
 "..KWWWWWWDWDDK..",
 "..KWWWWDDDWWWK..",
 "..KLLLLLDLLLLK..",
 "..KKKKKKKKKKKK.."] + qlegs('WL')
COW = quad(cow_side, cow_front, cow_back, 'W', 'L', [2, 8], ([1, 9], [3, 7]), ([3, 7], [1, 9]))

# ---------------------------------------------------------------- PIG
pig_side = [E, E, E, E,
 "..........KKQK..",
 ".KKKKKKKKKPPPPK.",
 "QKPPPPPPPPPPKPPK",
 "QKPPPPPPPPPPPQQK",
 ".KPPPPPPPPPPPDQK",
 ".KPPPPPPPPPKKKK.",
 ".KQQQQQQQQQK....",
 ".KKKKKKKKKKK....", E, E, E, E]
pig_front = [E, E,
 "...KK......KK...",
 "...KQKKKKKKQK...",
 "...KPPPPPPPPK...",
 "...KPDPPPPDPK...",
 "..KKPPKKKKPPKK..",
 "..KPPKQQQQKPPK..",
 "..KPPKQDDQKPPK..",
 "..KPPPKKKKPPPK..",
 "..KPPPPPPPPPPK..",
 "..KQQQQQQQQQQK..",
 "..KKKKKKKKKKKK.."] + qlegs('PQ')
pig_back = [E, E,
 "...KK......KK...",
 "...KQKKKKKKQK...",
 "...KPPPPPPPPK...",
 "...KPPPPPPPPK...",
 "..KKPPPPPPPPKK..",
 "..KPKKKKKKKKPK..",
 "..KPPPPPPPPPPK..",
 "..KPPPPPQPPPPK..",
 "..KPPPPQPQPPPK..",
 "..KQQQQQQQQQQK..",
 "..KKKKKKKKKKKK.."] + qlegs('PQ')
PIG = quad(pig_side, pig_front, pig_back, 'P', 'Q', [2, 8], ([1, 9], [3, 7]), ([3, 7], [1, 9]))

# ---------------------------------------------------------------- SHEEP
sheep_side = [E, E,
 "..KK.KKK.KK.....",
 ".KWWKWWWKWWK.KK.",
 "KWWWWWWWWWWWKTTK",
 "KWWLWWWWLWWWKTDK",
 "KWWWWLWWWWWLKTTK",
 "KWLWWWWWWLWWWKKK",
 "KWWWLWWWWWWLWK..",
 ".KWWWWLWWLWWWK..",
 ".KLLWLLWLLWLLK..",
 "..KKKKKKKKKKK...", E, E, E, E]
sheep_front = [E,
 "...KK.KKKK.KK...",
 "..KWWKWWWWKWWK..",
 ".KWWWKKKKKKWWWK.",
 ".KWWKTTTTTTKWWK.",
 ".KWLKTDTTDTKLWK.",
 ".KWWKTTTTTTKWWK.",
 ".KWWLKTMMTKLWWK.",
 ".KWLWWKKKKWWLWK.",
 ".KWWWLWWWWLWWWK.",
 ".KLWWWWLWWWWWLK.",
 "..KLLWLLLLWLLK..",
 "..KKKKKKKKKKKK.."] + qlegs('DM')
sheep_back = [E,
 "...KK.KKKK.KK...",
 "..KWWKWWWWKWWK..",
 ".KWWWWWWWWWWWWK.",
 ".KWWLWWWWWWLWWK.",
 ".KWWWWWLWWWWWWK.",
 ".KWLWWWWWWWWLWK.",
 ".KWWWWWWLWWWWWK.",
 ".KWWWLWWWWWLWWK.",
 ".KWWWWWWWWWWWWK.",
 ".KLWWWLWWWLWWLK.",
 "..KLLWLLLLWLLK..",
 "..KKKKKKKKKKKK.."] + qlegs('DM')
SHEEP = quad(sheep_side, sheep_front, sheep_back, 'D', 'M', [3, 9], ([2, 10], [4, 8]), ([4, 8], [2, 10]))
SHEARED = [[r.replace('W', 'P').replace('L', 'P') for r in f] for f in SHEEP]

# ---------------------------------------------------------------- CHICKEN
def chick_side(near):
    g = [E, E,
     "..........RR....",
     ".........KRRK...",
     ".........KWWK...",
     ".........KWDKYY.",
     "..KK.....KWWKR..",
     ".KWWKKKKKWWWK...",
     ".KWWWWWWWWWWK...",
     ".KWLLWWWWWWLK...",
     "..KWLLLLLLLK....",
     "...KKKKKKKK.....", E, E, E, E]
    g = [list(r) for r in g]
    for x, dx in near:
        g[12][x] = 'Y'; g[13][x + dx] = 'Y'; g[14][x + dx] = 'Y'; g[14][x + dx + 1] = 'Y'
    return [''.join(r) for r in g]
chick_front = [E, E, E,
 "......KRRK......",
 ".....KWWWWK.....",
 ".....KDWWDK.....",
 ".....KWYYWK.....",
 "...KKKWRRWKKK...",
 "..KWWWWWWWWWWK..",
 "..KWLWWWWWWLWK..",
 "..KWLLWWWWLLWK..",
 "...KWLLLLLLWK...",
 "....KKKKKKKK....",
 ".....Y....Y.....",
 "....YY.....Y....", E]
chick_back = [E, E, E,
 "......KRRK......",
 ".....KWWWWK.....",
 ".....KWWWWK.....",
 ".....KWWWWK.....",
 "...KKKWWWWKKK...",
 "..KWWWWWWWWWWK..",
 "..KWLWWWWWWLWK..",
 "..KWLLWLLWLLWK..",
 "...KWLLWWLLWK...",
 "....KKKKKKKK....",
 ".....Y....Y.....",
 "....YY.....Y....", E]
CHICKEN = [chick_front, chick_back, chick_side([(5, 0), (8, 0)]),
           chick_side([(5, -1), (8, 1)]), chick_side([(5, 1), (8, -1)])]

# ---------------------------------------------------------------- WOLF
wolf_side = [E,
 "...........K.K..",
 "..........KGKGK.",
 "K.........KGGGGK",
 "KK.KKKKKKKKGDGGK",
 ".KKMMMMMMMKGGLLK",
 "..KGGGMMMGGGKLLK",
 "..KGGGGGGGGGGKKK",
 "..KLGGGGGGGGGK..",
 "..KLLLLLLLLLLK..",
 "..KKKKKKKKKKKK..", E, E, E, E, E]
wolf_front = [E,
 "....K......K....",
 "...KGK....KGK...",
 "...KGGKKKKGGK...",
 "...KGGGGGGGGK...",
 "...KGDGGGGDGK...",
 "...KGGLLLLGGK...",
 "..KMKGLKKLGKMK..",
 "..KMMKLLLLKMMK..",
 "..KGGGKKKKGGGK..",
 "..KLGGGGGGGGLK..",
 "..KLLLGGGGLLLK..",
 "..KKKKKKKKKKKK.."] + qlegs('GM')
wolf_back = [E,
 "....K......K....",
 "...KGK....KGK...",
 "...KGGKKKKGGK...",
 "...KGGGGGGGGK...",
 "...KMMMMMMMMK...",
 "..KKMMMMMMMMKK..",
 "..KGKKKKKKKKGK..",
 "..KGMMMMMMMMGK..",
 "..KGGMMLLMMGGK..",
 "..KGGGGLLGGGGK..",
 "..KLLLLLLLLLLK..",
 "..KKKKKKKKKKKK.."] + qlegs('GM')
WOLF = quad(wolf_side, wolf_front, wolf_back, 'G', 'M', [3, 10], ([2, 11], [4, 9]), ([4, 9], [2, 11]),
            top=11, length=3)

# ---------------------------------------------------------------- SKELETON
skel_front = [E,
 ".....KKKKKK.....",
 "....KWWWWWWK....",
 "....KWKWWKWK....",
 "....KWWWWWWK....",
 ".....KWKKWK.....",
 "......KKKK......",
 "...KLKWWWWKLK...",
 "...KLKWKKWKLK...",
 "...KLKWWWWKLK...",
 "...KWKKLLKKWK...",
 "....K.KLLK.K....",
 "......KLLK......",
 ".....KLK.KLK....",
 ".....KLK.KLK....",
 ".....KKK..KK...."]
skel_back = [E,
 ".....KKKKKK.....",
 "....KWWWWWWK....",
 "....KWWWWWWK....",
 "....KLWWWWLK....",
 ".....KWWWWK.....",
 "......KKKK......",
 "...KLKWLLWKLK...",
 "...KLKWKKWKLK...",
 "...KLKWLLWKLK...",
 "...KWKKLLKKWK...",
 "....K.KLLK.K....",
 "......KLLK......",
 ".....KLK.KLK....",
 ".....KLK.KLK....",
 ".....KKK..KK...."]
SK_TOP = [E,
 "......KKKKK.....",
 ".....KWWWWWK....",
 ".....KWWWKWK....",
 ".....KWWWWWK....",
 "......KWKWK.....",
 "......KKKK......",
 "......KWWK......",
 "......KWLK......"]
skel_side = SK_TOP + [
 "......KWLK......",
 "......KLWK......",
 "......KWWK......",
 "......KLLK......",
 ".......KLK......",
 ".......KLK......",
 ".......KKK......", E][:8]
skel_side = SK_TOP + [
 "......KWLK......",
 "......KLWK......",
 "......KWWK......",
 "......KLLK......",
 ".......KLK......",
 ".......KLK......",
 ".......KKK......"]
skel_wa = SK_TOP + [
 "......KWLKLK....",
 "......KLWKLK....",
 "......KWWKK.....",
 "......KLLK......",
 ".....KLKKLK.....",
 "....KLK..KLK....",
 "....KK....KK...."]
skel_wb = SK_TOP + [
 "....KLKWLK......",
 "....KLKLWK......",
 ".....KKWWK......",
 "......KLLK......",
 "......KLKLK.....",
 "......KLKKLK....",
 "......KK..KK...."]
SKELETON = [skel_front, skel_back, skel_side, skel_wa, skel_wb]

# ---------------------------------------------------------------- SPIDER
spider_front = [E, E,
 ".K............K.",
 "..K...KKKK...K..",
 "...K.KBBBBK.K...",
 ".K..KBAAAABK..K.",
 "..KKKARAARAKKK..",
 "....KAAAAAAK....",
 ".KKKKAAAAAAKKKK.",
 "K...KAABBAAK...K",
 "..KKKAAAAAAKKK..",
 ".K..KBAAAABK..K.",
 "K....KAAAAK....K",
 "......KKKK......",
 "..........K.....", E]
spider_back = [E, E,
 ".K............K.",
 "..K...KKKK...K..",
 "...K.KBBBBK.K...",
 ".K..KBAAAABK..K.",
 "..KKKAAAAAAKKK..",
 "....KAAYYAAK....",
 ".KKKKAYAAYAKKKK.",
 "K...KAAYYAAK...K",
 "..KKKAAAAAAKKK..",
 ".K..KBAAAABK..K.",
 "K....KAAAAK....K",
 "......KKKK......",
 ".....K..........", E]
SP_BODY = [E, E, E, E,
 "......KKKKK.....",
 "....KKBBBBBKKK..",
 "...KBAAAAAAKRRK.",
 "...KAAABAAAKAAK.",
 "...KAAAAAAAKKKK.",
 "....KKKKKKKK...."]
spider_side = SP_BODY + [
 "...K.K..K.K..K..",
 "..K..K..K..K..K.",
 ".K...K..K...K..K",
 E, E, E]
spider_wa = SP_BODY + [
 "...K..K.K..K.K..",
 "..K..K..K.K...K.",
 "..K.K...K.K...K.",
 E, E, E]
spider_wb = SP_BODY + [
 "....K.K..K.K.K..",
 "...K..K..K..K.K.",
 "..K...K...K.K..K",
 E, E, E]
SPIDER = [spider_front, spider_back, spider_side, spider_wa, spider_wb]

# ---------------------------------------------------------------- BLOATER (exploding fungus)
CAP = [E,
 ".....KKKKKK.....",
 "...KKHHFFHHKK...",
 "..KHHFFYFFFHHK..",
 ".KHFYFFFFFYFFHK.",
 ".KFFFFFFYFFFFFK.",
 ".KFFYFFFFFFFYFK.",
 "..KKKKKKKKKKKK.."]
bloat_front = CAP + [
 "...KEEEEEEEEK...",
 "...KEKEEEEKEK...",
 "...KEEEKKEEEK...",
 "...KEEEEEEEEK...",
 "....KKKKKKKK....",
 ".....KE..EK.....",
 ".....KK...KK....", E]
bloat_back = CAP + [
 "...KEEEEEEEEK...",
 "...KEEEFEEEEK...",
 "...KEEEEEFEEK...",
 "...KEEEEEEEEK...",
 "....KKKKKKKK....",
 ".....KE..EK.....",
 ".....KK...KK....", E]
BL_SIDE = CAP + [
 "....KEEEEEEEK...",
 "....KEEEEEKEK...",
 "....KEEEEEEKK...",
 "....KEEEEEEEK...",
 ".....KKKKKKK...."]
bloat_side = BL_SIDE + ["......KE.EK.....", "......KK.KK.....", E]
bloat_wa = BL_SIDE + [".....KE...EK....", ".....KK...KK....", E]
bloat_wb = BL_SIDE + [".......KEK......", ".......KKK......", E]
BLOATER = [bloat_front, bloat_back, bloat_side, bloat_wa, bloat_wb]

# ---------------------------------------------------------------- MAGMA GOLEM (boss)
golem_front = [E,
 "....KKKKKKKK....",
 "...KMMMMMMMMK...",
 "...KMYYMMYYMK...",
 "...KDMMOMMMDK...",
 ".KKKKDMMMMDKKKK.",
 "KMMMKDDOODDKMMMK",
 "KMDMKMOYYOMKMDMK",
 "KMDMKMOYYOMKMDMK",
 "KDDDKDDOODDKDDDK",
 "KMOMKDMMMMDKMOMK",
 ".KKKKDDDDDDKKKK.",
 "....KDDKKDDK....",
 "....KMMK.KMMK...",
 "....KDDK.KDDK...",
 "....KKKK..KKK..."]
golem_back = [E,
 "....KKKKKKKK....",
 "...KMMMMMMMMK...",
 "...KMMMMMMMMK...",
 "...KDMMMMMMDK...",
 ".KKKKDMMMMDKKKK.",
 "KMMMKDMOMMDKMMMK",
 "KMDMKMMOOMMKMDMK",
 "KMDMKMOMMOMKMDMK",
 "KDDDKDOMMMDKDDDK",
 "KMMMKDMMMMDKMMMK",
 ".KKKKDDDDDDKKKK.",
 "....KDDKKDDK....",
 "....KMMK.KMMK...",
 "....KDDK.KDDK...",
 "....KKKK..KKK..."]
GO_TOP = [E,
 ".....KKKKKK.....",
 "....KMMMMMMK....",
 "....KMMMMYYK....",
 "....KDMMMMMK....",
 "....KKDDDDKK....",
 "...KDMKMMKMDK...",
 "...KDMKMMKODK...",
 "...KDMKDDKODK...",
 "...KDDKMOKDDK...",
 "....KDKMMKDK....",
 "....KKKKKKKK...."]
golem_side = GO_TOP + [".....KDDDDK.....", ".....KMMMMK.....", ".....KDDDDK.....", ".....KKKKKK....."]
golem_wa = GO_TOP + ["....KDDKKDDK....", "...KMMK..KMMK...", "...KDDK..KDDK...", "...KKKK..KKKK..."]
golem_wb = GO_TOP + ["......KDDK......", ".....KMMMMK.....", ".....KDDDDK.....", ".....KKKKKK....."]
GOLEM = [golem_front, golem_back, golem_side, golem_wa, golem_wb]

# ---------------------------------------------------------------- EXTRAS
arrow_h = [E, E, E, E, E, E,
 "..L.........K...",
 "..LLTTTTTTTTLK..",
 "..L.........K...",
 E, E, E, E, E, E, E]
arrow_v = [''.join(arrow_h[15 - x][y] for x in range(16)) for y in range(16)]
arrow_v = [r[::-1] for r in [''.join(arrow_h[x][15 - y] for x in range(16)) for y in range(16)]]
orb = [E, E, E, E,
 "..........Y.....",
 ".......KK.......",
 "......KYYK......",
 ".....KYYOOK.....",
 ".....KYOORK.....",
 "......KORK......",
 ".......KK.......",
 "....Y...........", E, E, E, E]
bed = [E,
 "..KKKKKKKKKKKK..",
 "..KbbbbbbbbbbK..",
 "..KcWWWWWWWWcK..",
 "..KcWWWWWWWWcK..",
 "..KcRRRRRRRRcK..",
 "..KcRRRRRRRRcK..",
 "..KcRRaRRaRRcK..",
 "..KcRRRRRRRRcK..",
 "..KcRRRRRRRRcK..",
 "..KcRRaRRaRRcK..",
 "..KcRRRRRRRRcK..",
 "..KcRRRRRRRRcK..",
 "..KbbbbbbbbbbK..",
 "..KKKKKKKKKKKK..", E]
table = [
 "...g.......g....",
 ".......g........",
 "....KKKKKKKK....",
 "....KpWWWWpK....",
 "....KpWuuWpK....",
 "..KKKKKKKKKKKK..",
 "..KuuuuuuuuuuK..",
 "..KDDDDDDDDDDK..",
 "..KDzzzzzzzzDK..",
 "..KDzgzzgzzzDK..",
 "..KDzzzgzzgzDK..",
 "..KDzzzzzzzzDK..",
 "..KDDDDDDDDDDK..",
 "..KKKKKKKKKKKK..", E, E]

MOBS = [  # (name, frames, legend)
 ('cow', COW, S8), ('pig', PIG, S8), ('chicken', CHICKEN, S8),
 ('sheep', SHEEP, S9), ('sheep (sheared)', SHEARED, S9), ('wolf', WOLF, S9), ('skeleton', SKELETON, S9),
 ('spider', SPIDER, S10), ('bloater', BLOATER, S10),
 ('magma golem', GOLEM, S11),
]
EXTRAS = [('arrow (horizontal)', arrow_h, S11), ('arrow (vertical)', arrow_v, S11),
          ('ember orb (XP)', orb, S11), ('bed', bed, S6), ('enchanting table', table, S6)]
