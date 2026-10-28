/* Copyright 2026 altmihai31
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef MINICRAFT_DEBUG_KIT
#define MINICRAFT_DEBUG_KIT

#include "main.h"

// Debug version only (make DEBUG_KIT=1): starting items and a shortcut
// down to test rooms on every underground level. See src/debug-kit.c.
#ifdef DEBUG_KIT

// fill the player's inventory with every mod item and gem tools
extern void debug_kit_give_items(void);

// carve the test rooms and stairs around the player's spawn point
extern void debug_kit_build_world(void);

// Stack high-water mark: debug_kit_stack_init fills the free stack with a
// pattern at boot; debug_kit_stack_free returns how many bytes between the
// end of IWRAM data and the deepest stack use were never touched. Shown in
// the performance overlay (hold L+R, press Select) as "S" + hex bytes.
extern void debug_kit_stack_init(void);
extern u32 debug_kit_stack_free(void);

#endif // DEBUG_KIT

#endif // MINICRAFT_DEBUG_KIT
