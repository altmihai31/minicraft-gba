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
#ifndef MINICRAFT_SURVIVAL
#define MINICRAFT_SURVIVAL

#include "main.h"

#include "level.h"

// Player state added by the mod: hunger, armor, experience and the
// bed spawn point. Everything here lives in EWRAM and runs from ROM.

#define MAX_HUNGER (10)

#define NO_SPAWN_POINT (0xff)

extern u8 player_hunger;
extern u16 player_hunger_timer;

extern u8 player_armor; // armor item type, or -1
extern u16 player_xp;

extern u8 spawn_level; // NO_SPAWN_POINT if no bed was used
extern u8 spawn_xt;
extern u8 spawn_yt;

// reset the mod's player state (new game, or death without
// 'keep inventory')
extern void survival_reset(bool reset_inventory);

// called once per tick by the player entity
extern void survival_player_tick(struct Level *level,
                                 struct entity_Data *data);

// returns true if the player's armor fully blocks a hit
extern bool survival_armor_blocks(void);

// on ice, keep moving in the last direction for a while
extern void survival_ice_slide(i32 *xm, i32 *ym);

// cast the fishing rod: a fish may bite after a random delay
extern void survival_start_fishing(void);

extern void survival_draw_status_bar(void);

// show 'text' (10 characters at most) for 2 seconds in place of the XP
// counter
extern void survival_show_message(const char *text, u8 palette);

#endif // MINICRAFT_SURVIVAL
