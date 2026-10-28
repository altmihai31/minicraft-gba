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
#include "player.h"

#include "level.h"
#include "tile.h"
#include "entity.h"
#include "mob.h"
#include "item.h"
#include "inventory.h"
#include "scene.h"
#include "survival.h"
#include "storage.h"
#include "sound.h"

// What the mod items do when the player presses A (item_use) or uses a
// bed or enchanting table with B (item_use_furniture). Runs from ROM.

// XP cost of enchant level 1 and 2
#define ENCHANT_COST_1 (30)
#define ENCHANT_COST_2 (60)

// arrow speed, in 1/64 pixels per tick
#define ARROW_SPEED (120)

static inline void set_active_item(u8 type) {
    player_active_item.type = type;
    player_active_item.count = 0;
}

// first entity of 'type' in front of the player
static struct entity_Data *facing_entity(struct Level *level,
                                         struct entity_Data *data,
                                         u8 type) {
    struct mob_Data *mob_data = (struct mob_Data *) &data->data;

    const u8 dir = mob_data->dir;
    const u8 range = 12;
    i32 x0 = data->x     - ((dir & 1) == 0) * 8 - (dir == 1) * range + (dir == 3) * 4;
    i32 y0 = data->y - 2 - ((dir & 1) == 1) * 8 - (dir == 0) * range + (dir == 2) * 4;
    i32 x1 = data->x     + ((dir & 1) == 0) * 8 + (dir == 3) * range - (dir == 1) * 4;
    i32 y1 = data->y - 2 + ((dir & 1) == 1) * 8 + (dir == 2) * range - (dir == 0) * 4;

    for(u32 i = 1; i < ENTITY_LIMIT; i++) {
        struct entity_Data *e_data = &level->entities[i];
        if(e_data->type != type)
            continue;

        if(entity_intersects(e_data, x0, y0, x1, y1))
            return e_data;
    }
    return NULL;
}

static bool has_resource(u8 type) {
    for(u32 i = 0; i < player_inventory.size; i++) {
        struct item_Data *item = &player_inventory.items[i];
        if(item->type == type && item->count > 0)
            return true;
    }
    return false;
}

static inline void use_bucket(struct Level *level, u32 xt, u32 yt) {
    const u32 tile = LEVEL_GET_TILE(level, xt, yt);
    const bool deepest = (level == &levels[0]);

    switch(player_active_item.type) {
        case BUCKET_ITEM:
            if(tile == LIQUID_TILE) {
                LEVEL_SET_TILE(level, xt, yt, HOLE_TILE, 0);
                set_active_item(deepest ? LAVA_BUCKET_ITEM
                                        : WATER_BUCKET_ITEM);
            }
            break;

        case WATER_BUCKET_ITEM:
            if(deepest) {
                // water on lava makes obsidian
                if(tile == LIQUID_TILE) {
                    LEVEL_SET_TILE(level, xt, yt, OBSIDIAN_TILE, 0);
                    entity_add_smash_particle(level, xt, yt);
                    set_active_item(BUCKET_ITEM);
                } else {
                    survival_show_message("CANT POUR", 2);
                }
            } else if(tile == HOLE_TILE || tile == DIRT_TILE ||
                      tile == GRASS_TILE || tile == SAND_TILE) {
                LEVEL_SET_TILE(level, xt, yt, LIQUID_TILE, 0);
                set_active_item(BUCKET_ITEM);
            } else {
                survival_show_message(tile == LIQUID_TILE ? "FULL"
                                                          : "CANT POUR", 2);
            }
            break;

        case LAVA_BUCKET_ITEM:
            // lava only flows on the deepest level
            if(deepest && (tile == HOLE_TILE || tile == DIRT_TILE)) {
                LEVEL_SET_TILE(level, xt, yt, LIQUID_TILE, 0);
                set_active_item(BUCKET_ITEM);
            } else {
                survival_show_message(tile == LIQUID_TILE ? "FULL"
                                                          : "CANT POUR", 2);
            }
            break;
    }
}

static inline void shoot_bow(struct Level *level, struct entity_Data *data) {
    if(!has_resource(ARROW_ITEM))
        return;
    inventory_remove_resource(&player_inventory, ARROW_ITEM, 1);

    struct mob_Data *mob_data = (struct mob_Data *) &data->data;
    const u8 dir = mob_data->dir;

    const i32 xd = (dir == 3) - (dir == 1);
    const i32 yd = (dir == 2) - (dir == 0);

    entity_add_arrow(
        level, data->x + xd * 6, data->y - 2 + yd * 6,
        xd * ARROW_SPEED, yd * ARROW_SPEED,
        true
    );
}

static inline void equip_armor(void) {
    const u8 old_armor = player_armor;

    player_armor = player_active_item.type;
    if(old_armor < ITEM_TYPES)
        set_active_item(old_armor);
    else
        player_active_item.type = -1;
}

THUMB
void item_use(struct Level *level, struct entity_Data *data) {
    i32 xt, yt;
    player_facing_tile(data, &xt, &yt);

    switch(player_active_item.type) {
        case BUCKET_ITEM:
        case WATER_BUCKET_ITEM:
        case LAVA_BUCKET_ITEM:
            use_bucket(level, xt, yt);
            break;

        case SHEARS_ITEM: {
            struct entity_Data *sheep = facing_entity(level, data, SHEEP_ENTITY);
            if(sheep)
                mod_mob_shear(level, sheep);
            break;
        }

        case BONE_ITEM: {
            struct entity_Data *wolf = facing_entity(level, data, WOLF_ENTITY);
            if(wolf && mod_mob_feed_bone(level, wolf)) {
                player_active_item.count--;
                if(player_active_item.count == 0)
                    player_active_item.type = -1;
            }
            break;
        }

        case BOW_ITEM:
            shoot_bow(level, data);
            break;

        case FISHING_ROD_ITEM:
            if(level != &levels[0] &&
               LEVEL_GET_TILE(level, xt, yt) == LIQUID_TILE) {
                entity_add_smash_particle(level, xt, yt);
                survival_start_fishing();
            }
            break;

        case MAP_ITEM:
            set_scene(&scene_map, 1);
            break;

        case LEATHER_ARMOR_ITEM:
        case IRON_ARMOR_ITEM:
        case GOLD_ARMOR_ITEM:
        case GEM_ARMOR_ITEM:
            equip_armor();
            SOUND_PLAY(sound_craft);
            break;
    }
}

THUMB
bool item_use_furniture(struct Level *level, struct entity_Data *data,
                        struct entity_Data *furniture) {
    if(furniture->type == BED_ENTITY) {
        // sleep: heal, respawn here, skip the night and save the game
        struct mob_Data *mob_data = (struct mob_Data *) &data->data;
        mob_data->hp = PLAYER_MAX_HP;

        spawn_level = current_level;
        spawn_xt = data->x >> 4;
        spawn_yt = data->y >> 4;

        survival_sleep();

        if(storage_save())
            survival_show_message("SAVED", 3);
        else
            survival_show_message("NOT SAVED", 2);

        SOUND_PLAY(sound_craft);
        return true;
    }

    if(furniture->type == ENCHANT_TABLE_ENTITY) {
        const bool is_tool = player_active_item.type < ITEM_TYPES &&
            ITEM_S(&player_active_item)->class == ITEMCLASS_TOOL;

        const u8 enchant_level = player_active_item.enchanted;
        const u16 cost = (enchant_level == 0) ? ENCHANT_COST_1
                                              : ENCHANT_COST_2;

        if(!is_tool) {
            survival_show_message("NEED TOOL", 2);
        } else if(enchant_level >= MAX_ENCHANT_LEVEL) {
            survival_show_message("MAX LEVEL", 2);
        } else if(player_xp < cost) {
            survival_show_message(enchant_level == 0 ? "NEED 30 XP"
                                                     : "NEED 60 XP", 2);
        } else {
            player_xp -= cost;
            player_active_item.enchanted = enchant_level + 1;

            survival_show_message(enchant_level == 0 ? "ENCHANTED!"
                                                     : "LEVEL II!", 3);
            SOUND_PLAY(sound_craft);
        }
        return true;
    }
    return false;
}
