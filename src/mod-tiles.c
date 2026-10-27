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
#include "tile.h"

#include "level.h"
#include "entity.h"
#include "screen.h"

// Drawing, ticking and light of the mod tiles. Everything here runs
// from ROM: the IWRAM tile loops only call in for tiles they don't know.

// palette row of each mod tile, see docs/art-pack.md
static const u8 mod_tile_palettes[TILE_TYPES - FIRST_MOD_TILE] = {
    2,  // coal ore
    3,  // obsidian
    0,  // planks
    2,  // stone bricks
    10, // clay bricks
    0,  // door (closed)
    0,  // door (open)
    5,  // pumpkin
    1,  // melon
    0,  // sugar cane
    10, // mushroom
    10, // torch
    7,  // spawner
    6   // ice
};

THUMB
void mod_tile_draw(struct Level *level, u32 xt, u32 yt, u16 tiles[4]) {
    const u32 tile = LEVEL_GET_TILE(level, xt, yt);
    if(tile < FIRST_MOD_TILE || tile >= TILE_TYPES)
        return;

    // every mod tile is a full 16x16 cell: cell = tile ID
    const u16 base = (tile * 4) |
                     mod_tile_palettes[tile - FIRST_MOD_TILE] << 12;
    tiles[0] = base;
    tiles[1] = base + 1;
    tiles[2] = base + 2;
    tiles[3] = base + 3;
}

static inline bool is_next_to_water(struct Level *level, i32 xt, i32 yt) {
    return LEVEL_GET_TILE(level, xt - 1, yt) == LIQUID_TILE ||
           LEVEL_GET_TILE(level, xt + 1, yt) == LIQUID_TILE ||
           LEVEL_GET_TILE(level, xt, yt - 1) == LIQUID_TILE ||
           LEVEL_GET_TILE(level, xt, yt + 1) == LIQUID_TILE;
}

// Sugar cane spreads along the water's edge
static inline void sugar_cane_tick(struct Level *level, u32 xt, u32 yt) {
    if(level == &levels[0] || random(8) != 0)
        return;

    i32 xn = xt;
    i32 yn = yt;
    if(random(2))
        xn += random(2) * 2 - 1;
    else
        yn += random(2) * 2 - 1;

    const u32 tile = LEVEL_GET_TILE(level, xn, yn);
    if((tile == GRASS_TILE || tile == SAND_TILE) &&
       is_next_to_water(level, xn, yn))
        LEVEL_SET_TILE(level, xn, yn, SUGAR_CANE_TILE, 0);
}

#define SPAWNER_RANGE     (8 * 16)
#define SPAWNER_MAX_MOBS  (3)

// A spawner keeps a few hostile mobs around while the player is near
static inline void spawner_tick(struct Level *level, u32 xt, u32 yt) {
    struct entity_Data *player = &level->entities[0];
    if(player->type >= ENTITY_TYPES)
        return;

    const i32 x = (xt << 4) + 8;
    const i32 y = (yt << 4) + 8;

    i32 xd = player->x - x;
    i32 yd = player->y - y;
    if(xd * xd + yd * yd > SPAWNER_RANGE * SPAWNER_RANGE)
        return;

    u32 mobs = 0;
    for(u32 i = 1; i < ENTITY_LIMIT; i++) {
        struct entity_Data *data = &level->entities[i];
        if(!entity_is_hostile(data->type))
            continue;

        xd = data->x - x;
        yd = data->y - y;
        if(xd * xd + yd * yd <= SPAWNER_RANGE * SPAWNER_RANGE)
            mobs++;
    }
    if(mobs >= SPAWNER_MAX_MOBS)
        return;

    // pick a free tile next to the spawner
    const i32 xs = xt + random(3) - 1;
    const i32 ys = yt + random(3) - 1;
    if(LEVEL_GET_TILE_S(level, xs, ys)->is_solid)
        return;

    const u16 sx = (xs << 4) + 8;
    const u16 sy = (ys << 4) + 8;
    switch(random(3)) {
        case 0:
            entity_add_mod_mob(level, SKELETON_ENTITY, sx, sy);
            break;
        case 1:
            entity_add_mod_mob(level, SPIDER_ENTITY, sx, sy);
            break;
        default:
            entity_add_zombie(level, sx, sy, 3 - (level - levels));
            break;
    }
}

THUMB
void mod_tile_tick(struct Level *level, u32 xt, u32 yt) {
    const u32 tile = LEVEL_GET_TILE(level, xt, yt);
    switch(tile) {
        case SUGAR_CANE_TILE:
            sugar_cane_tick(level, xt, yt);
            break;

        case SPAWNER_TILE:
            spawner_tick(level, xt, yt);
            // fall through: also recover damage

        case OBSIDIAN_TILE:
        case PLANKS_TILE:
        case STONE_BRICKS_TILE:
        case CLAY_BRICKS_TILE:
        case DOOR_CLOSED_TILE:
        case DOOR_OPEN_TILE:
        case PUMPKIN_TILE:
        case MELON_TILE:
        case ICE_TILE: {
            const u8 damage = LEVEL_GET_DATA(level, xt, yt);
            if(damage != 0)
                LEVEL_SET_DATA(level, xt, yt, damage - 1);
            break;
        }
    }
}

// Torches light the area around them, like a smaller lantern
THUMB
void mod_tile_draw_torch_light(struct Level *level) {
    const u32 x0 = level_x_offset >> 4;
    const u32 y0 = level_y_offset >> 4;

    for(i32 y = -3; y < 10 + 3; y++) {
        for(i32 x = -3; x < 16 + 3; x++) {
            if(LEVEL_GET_TILE(level, x0 + x, y0 + y) != TORCH_TILE)
                continue;

            const i32 xr = x * 2 + 1;
            const i32 yr = y * 2 + 1;

            i32 xl0 = xr - 6;
            i32 yl0 = yr - 6;
            i32 xl1 = xr + 6;
            i32 yl1 = yr + 6;

            if(xl1 < 0 || yl1 < 0)
                continue;

            if(xl0 < 0) xl0 = 0;
            if(yl0 < 0) yl0 = 0;
            if(xl1 > 32) xl1 = 32;
            if(yl1 > 20) yl1 = 20;

            for(i32 yl = yl0; yl < yl1; yl++) {
                u32 yd = (yl - yr + (yl >= yr)) * (yl - yr + (yl >= yr));

                for(i32 xl = xl0; xl < xl1; xl++) {
                    u32 xd = (xl - xr + (xl >= xr)) * (xl - xr + (xl >= xr));

                    u32 val = 224 + (xd + yd <= 46) + (xd + yd <= 30);
                    if(BG2_TILEMAP[xl + yl * 32] < val)
                        BG2_TILEMAP[xl + yl * 32] = val;
                }
            }
        }
    }
}
