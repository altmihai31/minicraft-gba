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
#include "debug-kit.h"

// Only compiled into the debug version (make DEBUG_KIT=1)
#ifdef DEBUG_KIT

#include "level.h"
#include "tile.h"
#include "entity.h"
#include "item.h"
#include "inventory.h"
#include "player.h"
#include "survival.h"

/*
    Layout, around the player's spawn tile (px, py):

    surface   grass clearing, a small pond to the left (fishing,
              buckets), animals around, stairs down at px+2
    level 2   lit room: stairs up at px+2, down at px+5,
              skeleton, spider and bloater at the far end
    level 1   lit room: stairs up at px+5, down at px+8, a spawner
    level 0   lit room: stairs up at px+8, a lava pool (water bucket ->
              obsidian) and the magma golem at the far end
*/

#define ROOM_X0 (-2)  // room extent, relative to px
#define ROOM_X1 (12)
#define ROOM_Y  (3)   // half height: torches on the walls light the middle

#define DEEP_ROOM_X1 (22) // the golem's room is longer

static void give_item(u8 type, u16 count, u8 tool_level) {
    struct item_Data item = { .type = type };

    if(ITEM_S(&item)->class == ITEMCLASS_TOOL)
        item.tool_level = tool_level;
    else if(item_is_resource(type))
        item.count = count;

    inventory_add(&player_inventory, &item, player_inventory.size);
}

THUMB
void debug_kit_give_items(void) {
    // gem tools at the top
    for(u32 type = SWORD_ITEM; type <= HOE_ITEM; type++)
        give_item(type, 0, 4);

    // every mod item
    for(u32 type = PLANKS_ITEM; type < ITEM_TYPES; type++)
        give_item(type, 99, 0);

    // crafting stations and raw materials for the recipes
    give_item(FURNACE_ITEM, 0, 0);
    give_item(OVEN_ITEM,    0, 0);
    give_item(ANVIL_ITEM,   0, 0);
    give_item(LANTERN_ITEM, 0, 0);

    const u8 materials[] = {
        WOOD_ITEM, STONE_ITEM, COAL_ITEM, WHEAT_ITEM,
        IRON_INGOT_ITEM, GOLD_INGOT_ITEM, GEM_ITEM
    };
    for(u32 i = 0; i < sizeof(materials); i++)
        give_item(materials[i], 99, 0);

    // enough for several enchantments
    player_xp = 300;
}

static void fill(struct Level *level, i32 x0, i32 y0, i32 x1, i32 y1,
                 u8 tile) {
    for(i32 y = y0; y <= y1; y++)
        for(i32 x = x0; x <= x1; x++)
            LEVEL_SET_TILE(level, x, y, tile, 0);
}

static void add_mob(struct Level *level, u8 type, i32 xt, i32 yt) {
    entity_add_mod_mob(level, type, (xt << 4) + 8, (yt << 4) + 8);
}

// an underground room with torches along its walls
static void carve_room(struct Level *level, i32 px, i32 py, i32 x1) {
    fill(level, px + ROOM_X0, py - ROOM_Y, px + x1, py + ROOM_Y, DIRT_TILE);

    for(i32 x = px + ROOM_X0; x <= px + x1; x += 3) {
        LEVEL_SET_TILE(level, x, py - ROOM_Y, TORCH_TILE, 0);
        LEVEL_SET_TILE(level, x, py + ROOM_Y, TORCH_TILE, 0);
    }
}

THUMB
void debug_kit_build_world(void) {
    struct Level *top = &levels[3];
    struct entity_Data *player = &top->entities[0];

    // keep every room inside the level
    i32 px = math_clip(player->x >> 4, 8, LEVEL_W - DEEP_ROOM_X1 - 2);
    i32 py = math_clip(player->y >> 4, 8, LEVEL_H - 8);

    player->x = (px << 4) + 8;
    player->y = (py << 4) + 8;

    // surface
    fill(top, px - 6, py - 4, px + 4, py + 4, GRASS_TILE);
    fill(top, px - 5, py - 1, px - 3, py + 1, LIQUID_TILE);
    LEVEL_SET_TILE(top, px + 2, py, STAIRS_DOWN_TILE, 0);

    add_mob(top, SHEEP_ENTITY,   px - 1, py - 3);
    add_mob(top, SHEEP_ENTITY,   px + 1, py - 3);
    add_mob(top, WOLF_ENTITY,    px + 3, py - 3);
    add_mob(top, COW_ENTITY,     px - 1, py + 3);
    add_mob(top, PIG_ENTITY,     px + 1, py + 3);
    add_mob(top, CHICKEN_ENTITY, px + 3, py + 3);

    // level 2
    struct Level *l2 = &levels[2];
    carve_room(l2, px, py, ROOM_X1);
    LEVEL_SET_TILE(l2, px + 2, py, STAIRS_UP_TILE, 0);
    LEVEL_SET_TILE(l2, px + 5, py, STAIRS_DOWN_TILE, 0);

    add_mob(l2, SKELETON_ENTITY, px + 11, py - 2);
    add_mob(l2, SPIDER_ENTITY,   px + 11, py);
    add_mob(l2, BLOATER_ENTITY,  px + 11, py + 2);

    // level 1
    struct Level *l1 = &levels[1];
    carve_room(l1, px, py, ROOM_X1);
    LEVEL_SET_TILE(l1, px + 5, py, STAIRS_UP_TILE, 0);
    LEVEL_SET_TILE(l1, px + 8, py, STAIRS_DOWN_TILE, 0);
    LEVEL_SET_TILE(l1, px + 11, py - 3, SPAWNER_TILE, 0);

    // level 0
    struct Level *l0 = &levels[0];
    carve_room(l0, px, py, DEEP_ROOM_X1);
    LEVEL_SET_TILE(l0, px + 8, py, STAIRS_UP_TILE, 0);
    fill(l0, px + 10, py + 1, px + 12, py + 3, LIQUID_TILE);

    // move the golem generated somewhere on this level to the far end
    for(u32 i = 1; i < ENTITY_LIMIT; i++) {
        struct entity_Data *data = &l0->entities[i];
        if(data->type == MAGMA_GOLEM_ENTITY) {
            data->x = ((px + DEEP_ROOM_X1 - 1) << 4) + 8;
            data->y = (py << 4) + 8;
        }
    }
}

#endif // DEBUG_KIT
