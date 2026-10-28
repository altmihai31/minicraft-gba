"""Check a GBA ROM header: title, game code, version and the complement
check (byte 0xBD) that the BIOS verifies before booting.

usage: python tools/check-header.py <rom.gba>
"""
import sys

rom = open(sys.argv[1], 'rb').read(0xC0)
want = (-sum(rom[0xA0:0xBD]) - 0x19) & 0xFF

print('title   ', rom[0xA0:0xAC].decode('ascii', 'replace'))
print('code    ', rom[0xAC:0xB0].decode('ascii', 'replace'))
print('version ', rom[0xBC])
print('checksum %#04x (expected %#04x)' % (rom[0xBD], want))

if rom[0xBD] != want:
    sys.exit('header checksum is wrong: set it to %#04x in src/header.s' % want)
print('header OK')
