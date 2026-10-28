# Building Minicraft Expanded Edition

The result is `bin/minicraft-expanded-edition.gba`, a ROM for any GBA emulator (mGBA recommended) or a
flash cart. The save type is 128 KB Flash.

## What you need
- The Arm GNU toolchain (`arm-none-eabi-gcc`), version 14.2 is what the project is measured with.
- Python 3 with Pillow (`pip install pillow`).
- GNU make and git.
- For testing: mGBA, and `arm-none-eabi-gdb` (comes with the toolchain) for `make test`.

## Linux and macOS
```sh
git clone --recursive https://github.com/altmihai31/minicraft-gba
cd minicraft-gba
make -C lib/libsimplegba
make res          # converts the PNGs in res/ to C sources in src/res/
make              # -> bin/minicraft-expanded-edition.gba
```
macOS: `brew install --cask gcc-arm-embedded`. Debian/Ubuntu: the Arm toolchain from developer.arm.com
(the distribution's `gcc-arm-none-eabi` is older and builds bigger code, which may not fit in IWRAM).

## Windows (Git Bash)
Install with winget: `Arm.GnuArmEmbeddedToolchain`, `Python.Python.3.12` (then `pip install pillow`),
`ezwinports.make`, `JeffreyPfau.mGBA`. Open a new Git Bash so the new PATH entries apply, then:
```sh
git clone --recursive https://github.com/altmihai31/minicraft-gba
cd minicraft-gba
make -C lib/libsimplegba AR=arm-none-eabi-ar
mkdir -p src/res/images src/res/palettes src/res/sounds
python tools/res2gba-win.py tools/res2gba/res2gba res/resources.json
make build OS=Linux
```
- `make res` doesn't work on Windows (its converter scripts are run through cmd.exe);
  `tools/res2gba-win.py` does the same conversion.
- `OS=Linux` makes the Makefile use the Unix-style commands, which Git Bash provides.
- If `python` opens the Microsoft Store, use `py` instead.
- mGBA locks the ROM it has open: close it before building, or the old ROM silently stays.

## Debug ROM
`make DEBUG_KIT=1` (Windows: `make build OS=Linux DEBUG_KIT=1`) builds
`bin/minicraft-expanded-edition-debug.gba`. A new game starts with every new item, gem tools and XP, next to
a pond, animals and stairs down through lit test rooms with the new monsters, a spawner, a lava pool and
the magma golem. It uses its own save file and object folder. Its performance overlay (hold L+R, press
Select) also shows the unused stack.

## Automated test
`make test` (Windows: `make test OS=Linux PYTHON=py`) plays the built normal ROM in a minimized mGBA through
its GDB stub and checks block placement, enchanting, day/night, beds, crafting and saving. See
`tools/test/smoke.py`. Set `MGBA` if mGBA isn't installed in the default place.

## Changing the art
Pixel art lives as text grids in `tools/art-source/`. After editing, run
`python tools/art-source/update_art.py` (writes `res/` and checks palette rows), convert the resources
again, and rebuild. `python tools/art-source/make_previews.py` refreshes `docs/previews/`. Details and
palette rules: [`docs/art-pack.md`](docs/art-pack.md).

## Screenshots
`python tools/test/screenshots.py` retakes the README screenshots (`docs/screenshots/`) with the debug ROM:
it drives mGBA like the test and composes each frame from the dumped video memory.

## Releases and CI
GitHub Actions (`.github/workflows/build.yml`) builds both ROMs on every push; the ROMs are attached to each
run. Pushing a tag `ee-vX.Y` also publishes a GitHub release with them. Steps:
[`doc/release-checklist.md`](doc/release-checklist.md). `python tools/check-header.py <rom>` checks the ROM
header checksum the GBA BIOS verifies.
