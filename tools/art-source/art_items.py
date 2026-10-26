"""New 8x8 item icons (items.png) + items palette additions."""
MAG = (255, 0, 255)

# legend per items-palette row (hex)
R0 = dict(a='211010', b='846b6b', c='ad9484', d='decece', e='7b6339', f='ad944a', g='ce9c6b',
          h='421010', i='631818', r='9c2929', o='de7b29', y='e7e76b', n='9c4a10')
R1 = dict(K='080808', D='393939', M='636363', G='949494', L='c6c6c6', W='f7f7f7', s='94947b',
          t='bdbd8c', k='102910', j='317331', m='429c42', n='4ac64a', R='c63939')
R2 = dict(Z='313118', O='8c8c42', B='b5b55a', Y='e7e76b', E='efefbd', U='8c8c63', P='e7e7a5',
          V='291029', X='8c298c', S='e7cee7', N='315aa5', G='63a563', H='8cd68c')
R3 = dict(a='211010', b='846b52', c='ad9463', D='393939', W='f7f7f7', y='e7e76b', q='4aadbd',
          i='c6c6de', j='9494ad', G='949494', L='c6c6c6', N='315aa5', o='de7b29', r='9c2929')

ITEMS_ADD = {0: ['de7b29', 'e7e76b', '9c4a10'],
             1: ['c63939'],
             3: ['c6c6de', '9494ad', '949494', 'c6c6c6', '315aa5', 'de7b29', '9c2929']}

def arm(L, A, B, C):
    g = ["AA....AA", "ABA..ABA", "ABBAABBA", ".ABBBBA.", ".ABCBBA.", ".ABBBBA.", ".ABBCBA.", ".AAAAAA."]
    return ([r.replace('A', A).replace('B', B).replace('C', C) for r in g], L)

ICONS = [
 ('planks', ["........", ".aaaaaa.", "agfgfffa", "aeeeeeea", "aaaaaaaa", "agffgffa", "aeeeeeea", ".aaaaaa."], R0),
 ('stone bricks', ["KKKKKKKK", "KWLLKWLK", "KLGGKLGK", "KKKKKKKK", "KLKWLLGK", "KGKLGGGK", "KKKKKKKK", "........"], R1),
 ('brick', ["........", "........", ".aaaaaa.", "agrrrrra", "arrrrria", "aiiiiiha", ".aaaaaa.", "........"], R0),
 ('clay', ["........", "..KKKK..", ".KttttK.", "KtWtttsK", "KttttssK", "KtssssMK", ".KssssK.", "..KKKK.."], R1),
 ('obsidian', ["........", "..VVVV..", ".VSXXXV.", "VSXXVXXV", "VXXVXXXV", "VXXXXXVV", ".VXXXVV.", "..VVVV.."], R2),
 ('door', [".aaaaaa.", ".afefea.", ".afefea.", ".afefda.", ".afefea.", ".afefea.", ".afefea.", ".aaaaaa."], R0),
 ('torch', ["...yy...", "..yooy..", "..orro..", "...fe...", "...fe...", "...fe...", "...fe...", "...aa..."], R0),
 ('pumpkin', ["....e...", "..aaeaa.", ".aoyonoa", "aoonoona", "aoonoona", "aoonoona", ".anoonna", "..aaaaa."], R0),
 ('melon slice', ["........", "........", ".KKKKKK.", "KRRKRRRK", "KRRRRKRK", ".KnnnnK.", "..KjjK..", "...KK..."], R1),
 ('sugar cane', [".nm..nm.", ".nm..nm.", ".jj..jj.", "nnm..nm.", ".nm..nmn", ".jj..jj.", ".nm..nm.", ".nm..nm."], R1),
 ('paper', ["........", ".KKKKK..", ".KWWWKK.", ".KWGGWK.", ".KWWWWK.", ".KWGGWK.", ".KWWWWK.", ".KKKKKK."], R1),
 ('mushroom', ["..aaaa..", ".ardrra.", "arrrrdra", "aiiiiiia", "..acda..", "..acca..", "..acca..", "..aaaa.."], R0),
 ('ice', [".jjjjjj.", "jWWiiiij", "jWiiiiij", "jiiWiiij", "jiWiiiij", "jiiiiiNj", "jiiiiNNj", ".jjjjjj."], R3),
 ('raw meat', ["........", "..aaa...", ".arrraa.", "ardrrrra", "arrdrrra", "arrrdrra", ".airrria", "..aaaaa."], R0),
 ('cooked meat', ["........", "..aaa...", ".aeeeaa.", "aegeeeea", "aeegeeea", "aeeegeea", ".ahhhhea", "..aaaaa."], R0),
 ('egg', ["........", "...aa...", "..adda..", ".adddca.", ".adddca.", ".addcca.", "..acca..", "...aa..."], R0),
 ('mushroom stew', ["........", "........", "aaaaaaaa", "abgbrbga", "aeffffea", ".aeffea.", "..aaaa..", "........"], R0),
 ('cake', ["........", "..r..r..", "aaaaaaaa", "adddddda", "adfdffda", "affffffa", "aeeeeeea", "aaaaaaaa"], R0),
 ('leather', [".a....a.", "aga..aga", ".agggga.", ".agffga.", ".agffga.", "aggffgga", "aga..aga", ".a....a."], R0),
 ('wool', ["..KKKK..", ".KWWLWK.", "KWWWWLLK", "KWLWWWLK", "KWWWLWWK", "KLWWWWLK", ".KLLWLK.", "..KKKK.."], R1),
 ('feather', [".....KKK", "....KWWK", "...KWLWK", "..KWLWK.", ".KWLWK..", ".KLWK...", ".DKK....", "D......."], R1),
 ('bone', [".KK.....", "KWWK....", "KWWLK...", ".KLWLK..", "..KLWLK.", "...KLWWK", "....KWWK", ".....KK."], R1),
 ('string', ["........", "...LLL..", "..L...L.", ".L..W.L.", ".L.L..L.", ".L..LL..", "..L....W", "...LLLW."], R1),
 ('spore powder', ["........", "....n...", "..n....n", "...KKK..", "..KmnmK.", ".KmnmmjK", "KjmmmjjK", "KKKKKKKK"], R1),
 ('bucket', ["..aaaa..", ".a....a.", "aaaaaaaa", "aLDDDDLa", "aWLLLGGa", ".aWLLGa.", ".aLLGGa.", "..aaaa.."], R3),
 ('water bucket', ["..aaaa..", ".a....a.", "aaaaaaaa", "aLNNiNLa", "aWLLLGGa", ".aWLLGa.", ".aLLGGa.", "..aaaa.."], R3),
 ('lava bucket', ["..aaaa..", ".a....a.", "aaaaaaaa", "aLoyoroa", "aWLLLGGa", ".aWLLGa.", ".aLLGGa.", "..aaaa.."], R3),
 ('shears', [".W...W..", ".LW.WL..", "..LWL...", "...G....", "..RGR...", ".R...R..", "R.R.R.R.", ".R...R.."], R1),
 ('bow', ["...ffd..", "..fe.d..", ".fe..d..", ".aa..d..", ".aa..d..", ".fe..d..", "..fe.d..", "...ffd.."], R0),
 ('arrow', [".....LLL", "......GL", ".....c.L", "....c...", "...c....", ".Wc.....", "W.W.....", ".W......"], R3),
 ('fishing rod', ["......cW", ".....c.W", "....c..W", "...c...W", "..c....W", ".b.....r", "b......W", "........"], R3),
 ('raw fish', ["........", "...ZZZ..", "Z.ZNNNZ.", "ZZNNNNVZ", "ZNNPPPNZ", "ZZPPPPZ.", "Z.ZZZZ..", "........"], R2),
 ('cooked fish', ["........", "...aaa..", "a.aeeea.", "aaeeeeha", "aeegggea", "aaggggа.".replace('а', 'a'), "a.aaaa..", "........"], R0),
 ('bed', [".aaaaaa.", ".adddda.", ".arrrra.", ".arirra.", ".arrrra.", ".arrrra.", ".aiiiia.", ".e....e."], R0),
 ('map', ["ZZZZZZZZ", "ZPPNNPPZ", "ZPGGNNPZ", "ZGGHGNPZ", "ZNGGGNNZ", "ZNNGHGNZ", "ZPNNGGPZ", "ZZZZZZZZ"], R2),
 ('leather armor',) + arm(R0, 'a', 'e', 'g'),
 ('iron armor',) + arm(R1, 'K', 'L', 'W'),
 ('gold armor',) + arm(R2, 'Z', 'B', 'Y'),
 ('gem armor',) + arm(R3, 'a', 'q', 'W'),
 ('enchanting table', ["...SS...", "..PPPP..", "VVVVVVVV", "VXXSXXXV", ".VXXXXV.", ".VXSXXV.", ".VXXXXV.", ".VVVVVV."], R2),
]
