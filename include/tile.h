/* Copyright 2022 Vulcalien
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
#ifndef MINICRAFT_TILE
#define MINICRAFT_TILE

#include "main.h"

#include "level.h"
#include "item.h"

#define TILE_TYPES (36)

#define GRASS_TILE          (0)
#define ROCK_TILE           (1)
#define LIQUID_TILE         (2)
#define FLOWER_TILE         (3)
#define TREE_TILE           (4)
#define DIRT_TILE           (5)
#define SAND_TILE           (6)
#define CACTUS_TILE         (7)
#define HOLE_TILE           (8)
#define TREE_SAPLING_TILE   (9)
#define CACTUS_SAPLING_TILE (10)
#define FARMLAND_TILE       (11)
#define WHEAT_TILE          (12)
#define STAIRS_DOWN_TILE    (13)
#define STAIRS_UP_TILE      (14)
#define INFINITE_FALL_TILE  (15)
#define CLOUD_TILE          (16)
#define HARD_ROCK_TILE      (17)
#define IRON_ORE_TILE       (18)
#define GOLD_ORE_TILE       (19)
#define GEM_ORE_TILE        (20)
#define CLOUD_CACTUS_TILE   (21)

// Mod tiles: the ID is also the cell in level.png (tiles 4*ID..4*ID+3)
#define FIRST_MOD_TILE      (22)
#define COAL_ORE_TILE       (22)
#define OBSIDIAN_TILE       (23)
#define PLANKS_TILE         (24)
#define STONE_BRICKS_TILE   (25)
#define CLAY_BRICKS_TILE    (26)
#define DOOR_CLOSED_TILE    (27)
#define DOOR_OPEN_TILE      (28)
#define PUMPKIN_TILE        (29)
#define MELON_TILE          (30)
#define SUGAR_CANE_TILE     (31)
#define MUSHROOM_TILE       (32)
#define TORCH_TILE          (33)
#define SPAWNER_TILE        (34)
#define ICE_TILE            (35)

// A torch breaks in one hit, so its data byte never holds damage: it keeps
// the ground the torch was placed on, drawn under it and left when broken.
#define TORCH_ON_DIRT  (0)
#define TORCH_ON_GRASS (1)
#define TORCH_ON_SAND  (2)

INLINE u8 torch_ground_of(u8 tile) {
    if(tile == GRASS_TILE || tile == FLOWER_TILE)
        return TORCH_ON_GRASS;
    if(tile == SAND_TILE)
        return TORCH_ON_SAND;
    return TORCH_ON_DIRT;
}

INLINE u8 torch_ground_tile(u8 ground) {
    if(ground == TORCH_ON_GRASS)
        return GRASS_TILE;
    if(ground == TORCH_ON_SAND)
        return SAND_TILE;
    return DIRT_TILE;
}

// Kept small: this table lives in IWRAM.
struct Tile {
    bool is_solid;
    u8 may_pass;

    u8 touch_damage;

    struct {
        u8 grass  : 1;
        u8 sand   : 1;
        u8 liquid : 1;
        u8 ice    : 1;
    } connects_to;

    void (*stepped_on)(struct Level *level, u32 xt, u32 yt,
                       struct entity_Data *entity_data);
};

static_assert(sizeof(struct Tile) == 8, "struct Tile: wrong size");

extern const struct Tile tile_list[TILE_TYPES];

typedef void (*tile_interact_fn)(struct Level *level, u32 xt, u32 yt,
                                 struct item_Data *item);

// in ROM: only used when the player hits a tile
extern const tile_interact_fn tile_interact_list[TILE_TYPES];

// mod tiles (src/mod-tiles.c, in ROM)
extern void mod_tile_draw(struct Level *level, u32 xt, u32 yt,
                          u16 tiles[4]);
extern void mod_tile_tick(struct Level *level, u32 xt, u32 yt);
extern void mod_tile_draw_torch_light(struct Level *level);

// open or close a door at (xt, yt), if there is one
extern bool tile_toggle_door(struct Level *level, u32 xt, u32 yt);

#endif // MINICRAFT_TILE
