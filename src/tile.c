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
#include "tile.h"

#include "level.h"
#include "entity.h"
#include "player.h"
#include "item.h"
#include "sound.h"

#define FSTEPPED_ON(name)\
    IWRAM_SECTION\
    static void name(struct Level *level, u32 xt, u32 yt,\
                     struct entity_Data *entity_data)

#define FINTERACT(name)\
    static void name(struct Level *level, u32 xt, u32 yt,\
                     struct item_Data *item)

#define GET_PUNCH_DAMAGE() (1 + random(3))

// Grass
FINTERACT(grass_interact) {
    if(item->type == SHOVEL_ITEM) {
        if(player_pay_stamina(4 - item->tool_level)) {
            if(random(5) == 0)
                entity_add_item(level, xt, yt, SEEDS_ITEM, true);

            LEVEL_SET_TILE(level, xt, yt, DIRT_TILE, 0);

            SOUND_PLAY(sound_monster_hurt);
        }
    } else if(item->type == HOE_ITEM) {
        if(player_pay_stamina(4 - item->tool_level)) {
            LEVEL_SET_TILE(level, xt, yt, FARMLAND_TILE, 0);

            if(random(5) == 0)
                entity_add_item(level, xt, yt, SEEDS_ITEM, true);

            SOUND_PLAY(sound_monster_hurt);
        }
    }
}

// Rock
FINTERACT(rock_interact) {
    u8 dmg;
    if(item->type == PICK_ITEM && player_pay_stamina(4 - item->tool_level))
        dmg = 10 + item->tool_level * 5 + random(10);
    else
        dmg = GET_PUNCH_DAMAGE();

    u8 damage = LEVEL_GET_DATA(level, xt, yt) + dmg;
    if(damage >= 50) {
        u8 count = 1 + random(4);
        for(u32 i = 0; i < count; i++)
            entity_add_item(level, xt, yt, STONE_ITEM, true);

        if(random(4) == 0)
            entity_add_item(level, xt, yt, COAL_ITEM, true);

        LEVEL_SET_TILE(level, xt, yt, DIRT_TILE, 0);
    } else {
        LEVEL_SET_DATA(level, xt, yt, damage);
    }

    entity_add_smash_particle(level, xt, yt);
    entity_add_text_particle(level, (xt << 4) + 8, (yt << 4) + 8, dmg, 0);
}

// Flower
FINTERACT(flower_interact) {
    u8 count;
    if(item->type == SHOVEL_ITEM && player_pay_stamina(4 - item->tool_level))
        count = 2;
    else
        count = 1 + random(2);

    for(u32 i = 0; i < count; i++)
        entity_add_item(level, xt, yt, FLOWER_ITEM, true);

    LEVEL_SET_TILE(level, xt, yt, GRASS_TILE, 0);
}

// Tree
FINTERACT(tree_interact) {
    u8 dmg;
    if(item->type == AXE_ITEM && player_pay_stamina(4 - item->tool_level))
        dmg = 10 + item->tool_level * 5 + random(10);
    else
        dmg = GET_PUNCH_DAMAGE();

    if(random(10) == 0)
        entity_add_item(level, xt, yt, APPLE_ITEM, true);

    u8 damage = LEVEL_GET_DATA(level, xt, yt) + dmg;
    if(damage >= 20) {
        u8 count = 1 + random(2);
        for(u32 i = 0; i < count; i++)
            entity_add_item(level, xt, yt, WOOD_ITEM, true);

        count = random(4);
        for(u32 i = 0; i < count; i++)
            entity_add_item(level, xt, yt, ACORN_ITEM, true);

        LEVEL_SET_TILE(level, xt, yt, GRASS_TILE, 0);
    } else {
        LEVEL_SET_DATA(level, xt, yt, damage);
    }

    entity_add_smash_particle(level, xt, yt);
    entity_add_text_particle(level, (xt << 4) + 8, (yt << 4) + 8, dmg, 0);
}

// Dirt
FINTERACT(dirt_interact) {
    if(item->type == SHOVEL_ITEM) {
        if(player_pay_stamina(4 - item->tool_level)) {
            entity_add_item(level, xt, yt, DIRT_ITEM, true);

            LEVEL_SET_TILE(level, xt, yt, HOLE_TILE, 0);

            SOUND_PLAY(sound_monster_hurt);
        }
    } else if(item->type == HOE_ITEM) {
        if(player_pay_stamina(4 - item->tool_level)) {
            LEVEL_SET_TILE(level, xt, yt, FARMLAND_TILE, 0);

            SOUND_PLAY(sound_monster_hurt);
        }
    }
}

// Sand
FSTEPPED_ON(sand_stepped_on) {
    u8 etype = entity_data->type;
    if(etype == ZOMBIE_ENTITY ||
       etype == SLIME_ENTITY ||
       etype == PLAYER_ENTITY) {
        LEVEL_SET_DATA(level, xt, yt, 10);
    }
}

FINTERACT(sand_interact) {
    if(item->type == SHOVEL_ITEM && player_pay_stamina(4 - item->tool_level)) {
        entity_add_item(level, xt, yt, SAND_ITEM, true);

        // sand by the water hides clay
        const bool by_water =
            LEVEL_GET_TILE(level, xt - 1, yt) == LIQUID_TILE ||
            LEVEL_GET_TILE(level, xt + 1, yt) == LIQUID_TILE ||
            LEVEL_GET_TILE(level, xt, yt - 1) == LIQUID_TILE ||
            LEVEL_GET_TILE(level, xt, yt + 1) == LIQUID_TILE;
        if(by_water && random(4) == 0)
            entity_add_item(level, xt, yt, CLAY_ITEM, true);

        LEVEL_SET_TILE(level, xt, yt, DIRT_TILE, 0);

        // no sound after digging sand
        // might be a bug in the original game
    }
}

// Cactus
FINTERACT(cactus_interact) {
    u8 dmg = GET_PUNCH_DAMAGE();

    u8 damage = LEVEL_GET_DATA(level, xt, yt) + dmg;
    if(damage >= 10) {
        u8 count = 1 + random(2);
        for(u32 i = 0; i < count; i++)
            entity_add_item(level, xt, yt, CACTUS_ITEM, true);

        LEVEL_SET_TILE(level, xt, yt, SAND_TILE, 0);
    } else {
        LEVEL_SET_DATA(level, xt, yt, damage);
    }

    entity_add_smash_particle(level, xt, yt);
    entity_add_text_particle(level, (xt << 4) + 8, (yt << 4) + 8, dmg, 0);
}

// Tree Sapling
FINTERACT(tree_sapling_interact) {
    LEVEL_SET_TILE(level, xt, yt, GRASS_TILE, 0);
}

// Cactus Sapling
FINTERACT(cactus_sapling_interact) {
    LEVEL_SET_TILE(level, xt, yt, SAND_TILE, 0);
}

// Farmland
FSTEPPED_ON(farmland_stepped_on) {
    if(random(60) != 0)
        return;

    if(LEVEL_GET_DATA(level, xt, yt) >= 5)
        LEVEL_SET_TILE(level, xt, yt, DIRT_TILE, 0);
}

FINTERACT(farmland_interact) {
    if(item->type == SHOVEL_ITEM && player_pay_stamina(4 - item->tool_level))
        LEVEL_SET_TILE(level, xt, yt, DIRT_TILE, 0);
}

// Wheat
static inline void wheat_harvest(struct Level *level, u32 xt, u32 yt) {
    if(random(2))
        entity_add_item(level, xt, yt, SEEDS_ITEM, true);

    u8 age = LEVEL_GET_DATA(level, xt, yt);
    if(age >= 40) {
        u8 count;
        if(age == 50)
            count = 2 + random(3);
        else
            count = 1 + random(2);

        for(u32 i = 0; i < count; i++)
            entity_add_item(level, xt, yt, WHEAT_ITEM, true);
    }

    LEVEL_SET_TILE(level, xt, yt, DIRT_TILE, 0);
}

FSTEPPED_ON(wheat_stepped_on) {
    if(random(60) != 0)
        return;

    if(LEVEL_GET_DATA(level, xt, yt) >= 2)
        wheat_harvest(level, xt, yt);
}

FINTERACT(wheat_interact) {
    if(item->type == SHOVEL_ITEM && player_pay_stamina(4 - item->tool_level))
        LEVEL_SET_TILE(level, xt, yt, DIRT_TILE, 0);
    else
        wheat_harvest(level, xt, yt);
}

// Cloud
FINTERACT(cloud_interact) {
    if(item->type == SHOVEL_ITEM && player_pay_stamina(5)) {
        u8 count = 1 + random(2);
        for(u32 i = 0; i < count; i++)
            entity_add_item(level, xt, yt, CLOUD_ITEM, true);
    }
}

// Hard Rock
FINTERACT(hard_rock_interact) {
    u8 dmg = 0;
    if(item->type == PICK_ITEM && item->tool_level == 4) {
        dmg = 30 + random(10);

        u8 damage = LEVEL_GET_DATA(level, xt, yt) + dmg;
        if(damage >= 200) {
            u8 count = 1 + random(4);
            for(u32 i = 0; i < count; i++)
                entity_add_item(level, xt, yt, STONE_ITEM, true);

            if(random(2))
                entity_add_item(level, xt, yt, COAL_ITEM, true);

            LEVEL_SET_TILE(level, xt, yt, DIRT_TILE, 0);
        } else {
            LEVEL_SET_DATA(level, xt, yt, damage);
        }
    }

    entity_add_smash_particle(level, xt, yt);
    entity_add_text_particle(level, (xt << 4) + 8, (yt << 4) + 8, dmg, 0);
}

// Ores
FINTERACT(ore_interact) {
    u8 dmg = 0;
    if(item->type == PICK_ITEM && player_pay_stamina(6 - item->tool_level)) {
        dmg = 1;

        u8 count = random(2);
        u8 tile = LEVEL_GET_TILE(level, xt, yt);
        u8 item_to_drop = ((tile == IRON_ORE_TILE) * IRON_ORE_ITEM) |
                          ((tile == GOLD_ORE_TILE) * GOLD_ORE_ITEM) |
                          ((tile == GEM_ORE_TILE)  * GEM_ITEM)      |
                          ((tile == COAL_ORE_TILE) * COAL_ITEM);

        u8 damage = LEVEL_GET_DATA(level, xt, yt) + dmg;
        if(damage >= 3 + random(10)) {
            LEVEL_SET_TILE(level, xt, yt, DIRT_TILE, 0);

            count += 2;

            // XP: coal and iron 1, gold 2, gem 3
            const u32 xp = 1 + (tile == GOLD_ORE_TILE) + 2 * (tile == GEM_ORE_TILE);
            for(u32 i = 0; i < xp; i++)
                entity_add_xp_orb(level, (xt << 4) + 8, (yt << 4) + 8);
        } else {
            LEVEL_SET_DATA(level, xt, yt, damage);
        }

        for(u32 i = 0; i < count; i++)
            entity_add_item(level, xt, yt, item_to_drop, true);
    }

    entity_add_smash_particle(level, xt, yt);
    entity_add_text_particle(level, (xt << 4) + 8, (yt << 4) + 8, dmg, 0);
}

// Cloud Cactus
FINTERACT(cloud_cactus_interact) {
    u8 dmg = 0;
    if(item->type == PICK_ITEM && player_pay_stamina(6 - item->tool_level)) {
        dmg = 1;

        u8 damage = LEVEL_GET_DATA(level, xt, yt) + dmg;
        if(damage >= 10)
            LEVEL_SET_TILE(level, xt, yt, CLOUD_TILE, 0);
        else
            LEVEL_SET_DATA(level, xt, yt, damage);
    }

    entity_add_smash_particle(level, xt, yt);
    entity_add_text_particle(level, (xt << 4) + 8, (yt << 4) + 8, dmg, 0);
}

/* ================================================================== */
/*                             Mod blocks                             */
/* ================================================================== */

// Tools that deal full damage to a block
#define BREAK_ANY  (0)
#define BREAK_AXE  (1 << 0)
#define BREAK_PICK (1 << 1)

#define DROP_NONE (0xff)
#define DROP_XP   (0xfe)

struct BlockBreak {
    u8 tool;
    u8 min_tier; // above 0: anything else deals no damage
    u8 hp;

    u8 drop;
    u8 drop_min;
    u8 drop_extra; // up to 'drop_extra' more, at random

    u8 leaves; // tile left behind
};

// One row per mod block, indexed by (tile - FIRST_MOD_TILE).
// Coal ore breaks like the other ores ('ore_interact').
static const struct BlockBreak block_breaks[TILE_TYPES - FIRST_MOD_TILE] = {
    [OBSIDIAN_TILE - FIRST_MOD_TILE] = {
        BREAK_PICK, 4, 200, OBSIDIAN_ITEM, 1, 0, DIRT_TILE
    },
    [PLANKS_TILE - FIRST_MOD_TILE] = {
        BREAK_AXE, 0, 20, PLANKS_ITEM, 1, 0, DIRT_TILE
    },
    [STONE_BRICKS_TILE - FIRST_MOD_TILE] = {
        BREAK_PICK, 0, 50, STONE_BRICKS_ITEM, 1, 0, DIRT_TILE
    },
    [CLAY_BRICKS_TILE - FIRST_MOD_TILE] = {
        BREAK_PICK, 0, 40, BRICK_ITEM, 1, 0, DIRT_TILE
    },
    [DOOR_CLOSED_TILE - FIRST_MOD_TILE] = {
        BREAK_AXE | BREAK_PICK, 0, 20, DOOR_ITEM, 1, 0, DIRT_TILE
    },
    [DOOR_OPEN_TILE - FIRST_MOD_TILE] = {
        BREAK_AXE | BREAK_PICK, 0, 20, DOOR_ITEM, 1, 0, DIRT_TILE
    },
    [PUMPKIN_TILE - FIRST_MOD_TILE] = {
        BREAK_AXE, 0, 10, PUMPKIN_ITEM, 1, 0, GRASS_TILE
    },
    [MELON_TILE - FIRST_MOD_TILE] = {
        BREAK_AXE, 0, 10, MELON_SLICE_ITEM, 3, 2, GRASS_TILE
    },
    [SUGAR_CANE_TILE - FIRST_MOD_TILE] = {
        BREAK_ANY, 0, 1, SUGAR_CANE_ITEM, 1, 1, GRASS_TILE
    },
    [MUSHROOM_TILE - FIRST_MOD_TILE] = {
        BREAK_ANY, 0, 1, MUSHROOM_ITEM, 1, 1, DIRT_TILE
    },
    [TORCH_TILE - FIRST_MOD_TILE] = {
        BREAK_ANY, 0, 1, TORCH_ITEM, 1, 0, DIRT_TILE
    },
    [SPAWNER_TILE - FIRST_MOD_TILE] = {
        BREAK_PICK, 2, 150, DROP_XP, 5, 5, DIRT_TILE
    },
    [ICE_TILE - FIRST_MOD_TILE] = {
        BREAK_PICK, 0, 20, ICE_ITEM, 1, 0, LIQUID_TILE
    }
};

static inline bool is_break_tool(const struct BlockBreak *b,
                                 struct item_Data *item) {
    return ((b->tool & BREAK_AXE)  && item->type == AXE_ITEM) ||
           ((b->tool & BREAK_PICK) && item->type == PICK_ITEM);
}

// The one break rule shared by all mod blocks
THUMB
FINTERACT(block_interact) {
    const u8 tile = LEVEL_GET_TILE(level, xt, yt);
    const struct BlockBreak *b = &block_breaks[tile - FIRST_MOD_TILE];

    u8 dmg;
    if(is_break_tool(b, item) && item->tool_level >= b->min_tier &&
       player_pay_stamina(4 - item->tool_level))
        dmg = 10 + item->tool_level * 5 + random(10);
    else if(b->min_tier == 0)
        dmg = GET_PUNCH_DAMAGE();
    else
        dmg = 0;

    u32 damage = LEVEL_GET_DATA(level, xt, yt) + dmg;
    if(damage >= b->hp) {
        const u32 count = b->drop_min + random(b->drop_extra + 1);

        if(b->drop == DROP_XP) {
            for(u32 i = 0; i < count; i++)
                entity_add_xp_orb(level, (xt << 4) + 8, (yt << 4) + 8);
        } else if(b->drop != DROP_NONE) {
            for(u32 i = 0; i < count; i++)
                entity_add_item(level, xt, yt, b->drop, true);
        }

        LEVEL_SET_TILE(level, xt, yt, b->leaves, 0);
    } else {
        LEVEL_SET_DATA(level, xt, yt, damage);
    }

    entity_add_smash_particle(level, xt, yt);
    if(b->hp > 1)
        entity_add_text_particle(level, (xt << 4) + 8, (yt << 4) + 8, dmg, 0);
}

THUMB
bool tile_toggle_door(struct Level *level, u32 xt, u32 yt) {
    const u8 tile = LEVEL_GET_TILE(level, xt, yt);
    if(tile != DOOR_CLOSED_TILE && tile != DOOR_OPEN_TILE)
        return false;

    // a door can't be closed on top of a solid entity
    if(tile == DOOR_OPEN_TILE) {
        for(u32 i = 0; i < SOLID_ENTITIES_IN_TILE; i++)
            if(level_solid_entities[xt + yt * LEVEL_W][i] < ENTITY_LIMIT)
                return true;
    }

    const u8 new_tile = (tile == DOOR_CLOSED_TILE) ? DOOR_OPEN_TILE
                                                  : DOOR_CLOSED_TILE;
    LEVEL_SET_TILE(level, xt, yt, new_tile, LEVEL_GET_DATA(level, xt, yt));
    return true;
}

// Axe or pick break the door, anything else opens or closes it
THUMB
FINTERACT(door_interact) {
    if(item->type == AXE_ITEM || item->type == PICK_ITEM)
        block_interact(level, xt, yt, item);
    else
        tile_toggle_door(level, xt, yt);
}

IWRAM_RODATA_SECTION
const struct Tile tile_list[TILE_TYPES] = {
    // Grass
    {
        .connects_to = {
            .grass = true
        }
    },

    // Rock
    {
        .is_solid = true,
        .may_pass = -1
    },

    // Liquid
    {
        .connects_to = {
            .sand   = true,
            .liquid = true
        },

        .is_solid = true,
        .may_pass = PLAYER_ENTITY
    },

    // Flower
    {
        .connects_to = {
            .grass = true
        }
    },

    // Tree
    {
        .connects_to = {
            .grass = true
        },

        .is_solid = true,
        .may_pass = -1
    },

    // Dirt
    { 0 },

    // Sand
    {
        .connects_to = {
            .sand = true
        },

        .stepped_on = sand_stepped_on
    },

    // Cactus
    {
        .connects_to = {
            .sand = true
        },

        .is_solid = true,
        .may_pass = -1,

        .touch_damage = 1
    },

    // Hole
    {
        .connects_to = {
            .sand   = true,
            .liquid = true,
        },

        .is_solid = true,
        .may_pass = PLAYER_ENTITY
    },

    // Tree Sapling
    {
        .connects_to = {
            .grass = true
        }
    },

    // Cactus Sapling
    {
        .connects_to = {
            .sand = true
        }
    },

    // Farmland
    {
        .stepped_on = farmland_stepped_on
    },

    // Wheat
    {
        .stepped_on = wheat_stepped_on
    },

    // Stairs Down
    { 0 },

    // Stairs Up
    { 0 },

    // Infinite Fall
    {
        .is_solid = true,
        .may_pass = AIR_WIZARD_ENTITY
    },

    // Cloud
    { 0 },

    // Hard Rock
    {
        .is_solid = true,
        .may_pass = -1
    },

    // Iron Ore
    {
        .is_solid = true,
        .may_pass = -1,

        .touch_damage = 3
    },

    // Gold Ore
    {
        .is_solid = true,
        .may_pass = -1,

        .touch_damage = 3
    },

    // Gem Ore
    {
        .is_solid = true,
        .may_pass = -1,

        .touch_damage = 3
    },

    // Cloud Cactus
    {
        .is_solid = true,
        .may_pass = AIR_WIZARD_ENTITY,

        .touch_damage = 3
    },

    // Coal Ore
    {
        .is_solid = true,
        .may_pass = -1
    },

    // Obsidian
    {
        .is_solid = true,
        .may_pass = -1
    },

    // Planks
    {
        .is_solid = true,
        .may_pass = -1
    },

    // Stone Bricks
    {
        .is_solid = true,
        .may_pass = -1
    },

    // Clay Bricks
    {
        .is_solid = true,
        .may_pass = -1
    },

    // Door (closed)
    {
        .connects_to = {
            .grass = true
        },

        .is_solid = true,
        .may_pass = -1
    },

    // Door (open)
    {
        .connects_to = {
            .grass = true
        }
    },

    // Pumpkin
    {
        .connects_to = {
            .grass = true
        },

        .is_solid = true,
        .may_pass = -1
    },

    // Melon
    {
        .connects_to = {
            .grass = true
        },

        .is_solid = true,
        .may_pass = -1
    },

    // Sugar Cane
    {
        .connects_to = {
            .grass = true
        }
    },

    // Mushroom
    { 0 },

    // Torch
    { 0 },

    // Spawner
    {
        .is_solid = true,
        .may_pass = -1
    },

    // Ice
    {
        .connects_to = {
            .ice = true
        }
    }
};


const tile_interact_fn tile_interact_list[TILE_TYPES] = {
    grass_interact,        // Grass
    rock_interact,         // Rock
    NULL,                  // Liquid
    flower_interact,       // Flower
    tree_interact,         // Tree
    dirt_interact,         // Dirt
    sand_interact,         // Sand
    cactus_interact,       // Cactus
    NULL,                  // Hole
    tree_sapling_interact, // Tree Sapling
    cactus_sapling_interact, // Cactus Sapling
    farmland_interact,     // Farmland
    wheat_interact,        // Wheat
    NULL,                  // Stairs Down
    NULL,                  // Stairs Up
    NULL,                  // Infinite Fall
    cloud_interact,        // Cloud
    hard_rock_interact,    // Hard Rock
    ore_interact,          // Iron Ore
    ore_interact,          // Gold Ore
    ore_interact,          // Gem Ore
    cloud_cactus_interact, // Cloud Cactus
    ore_interact,          // Coal Ore
    block_interact,        // Obsidian
    block_interact,        // Planks
    block_interact,        // Stone Bricks
    block_interact,        // Clay Bricks
    door_interact,         // Door (closed)
    door_interact,         // Door (open)
    block_interact,        // Pumpkin
    block_interact,        // Melon
    block_interact,        // Sugar Cane
    block_interact,        // Mushroom
    block_interact,        // Torch
    block_interact,        // Spawner
    block_interact         // Ice
};
