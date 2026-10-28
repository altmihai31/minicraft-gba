# Changelog

## [Expanded Edition 1.0] - 28.10.2026
First release of Minicraft Expanded Edition, a fork of Minicraft for GBA 2.0.
**(!) Saves use a new 128 KB Flash layout: Minicraft for GBA saves can't be
loaded. (!)**

### Added
- 13 blocks: coal ore, obsidian, planks, stone bricks, clay bricks, doors,
  pumpkins, melons, sugar cane, mushrooms, torches, spawners and ice.
  Placed planks and bricks join into walls like rock; doors fit into them.
- 9 mobs: cow, pig, chicken, sheep, wolf (tameable), skeleton, spider,
  bloater and the magma golem boss on the deepest level.
- 40 items: buckets, shears, bow and arrows, fishing rod, map, bed,
  enchanting table, four armor tiers, new food and materials, with
  workbench, furnace, oven and anvil recipes.
- Hunger, armor, experience and enchanting (levels I and II).
- Day and night on the surface: torches, lanterns and the player light
  the dark, monsters spawn at night. Sleeping in a bed skips the night,
  sets the respawn point and saves the game.
- Status messages in the HUD for actions that fail or succeed.
- Recipe result counts in the crafting menu.
- Debug build (`make DEBUG_KIT=1`) and an automated smoke test
  (`make test`).

### Changed
- Building blocks, doors and torches can be placed on grass, dirt, sand,
  flowers, farmland and wheat; a flower or crop underneath drops.
- Saves use 128 KB Flash (two banks), so large worlds fit.

### Fixed
- Crafting counts a resource split over several inventory slots.
- A solid block can't be placed on a mob, furniture or a dropped item.

## [2.0] - 2026-02-19
### Changed
- Rework the save file layout and switch to 64 KB Flash ROM.
  **(!) Save files from previous versions are incompatible. (!)**
- Reduce inventory capacity from 128 to 96 items.
- Accept both *B* and *START* buttons to exit menus.
- Store game images as PNG files instead of hex-code, so that they can
  be viewed and modified without special tools.
- Use the *libsimplegba* library to handle many low-level and basic
  things.
- Use more accurate sine and cosine functions when the Air Wizard shoots
  sparks.

### Fixed
- Menus flickering when holding the menu's toggle buttons.

## [1.3] - 2024-02-09
### Added
- Checksum verification to validate save files.
- Options menu.
- Respawn feature, with an option to keep the inventory.

### Changed
- Use a faster algorithm to sort entities, reducing the time needed to
  update the screen.

### Fixed
- Time displayed in pause, death and win menus, which was slightly lower
  than it should be.

## [1.2.1] - 2023-05-04
### Fixed
- The linker script, which was ignoring some input sections.

## [1.2] - 2023-05-01
### Added
- Frameskip to lower the fps when necessary.
- TPS and FPS counters in the performance overlay.

### Fixed
- Visual bug that caused trailing zeros to appear after the item count
  after losing a decimal digit. (Introduced in 1.1)
- Visual bug that caused some hearts to be drawn even after the player
  dies in lava.

## [1.1] - 2023-03-10
### Fixed
- Entities being drawn on top of the darkness layer when close to a
  player holding a lantern.

## [1.0] - 2022-11-19
First release of the game.
