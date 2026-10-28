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
#ifndef MINICRAFT_ITEM
#define MINICRAFT_ITEM

#include "main.h"

#define ITEM_TYPES (73)

#define WOOD_ITEM       (0)
#define STONE_ITEM      (1)
#define GLASS_ITEM      (2)
#define WHEAT_ITEM      (3)
#define SLIME_ITEM      (4)
#define CLOTH_ITEM      (5)
#define COAL_ITEM       (6)
#define IRON_ORE_ITEM   (7)
#define GOLD_ORE_ITEM   (8)
#define IRON_INGOT_ITEM (9)
#define GOLD_INGOT_ITEM (10)
#define GEM_ITEM        (11)

#define FLOWER_ITEM (12)
#define SEEDS_ITEM  (13)
#define ACORN_ITEM  (14)
#define CACTUS_ITEM (15)
#define DIRT_ITEM   (16)
#define SAND_ITEM   (17)
#define CLOUD_ITEM  (18)

#define APPLE_ITEM (19)
#define BREAD_ITEM (20)

#define WORKBENCH_ITEM (21)
#define FURNACE_ITEM   (22)
#define OVEN_ITEM      (23)
#define ANVIL_ITEM     (24)
#define CHEST_ITEM     (25)
#define LANTERN_ITEM   (26)

#define POWERGLOVE_ITEM (27)

#define SWORD_ITEM  (28)
#define AXE_ITEM    (29)
#define PICK_ITEM   (30)
#define SHOVEL_ITEM (31)
#define HOE_ITEM    (32)

// Mod items: icon = type + 22 (icons 30-54 are the tool tiers)
#define PLANKS_ITEM         (33)
#define STONE_BRICKS_ITEM   (34)
#define BRICK_ITEM          (35)
#define CLAY_ITEM           (36)
#define OBSIDIAN_ITEM       (37)
#define DOOR_ITEM           (38)
#define TORCH_ITEM          (39)
#define PUMPKIN_ITEM        (40)
#define MELON_SLICE_ITEM    (41)
#define SUGAR_CANE_ITEM     (42)
#define PAPER_ITEM          (43)
#define MUSHROOM_ITEM       (44)
#define ICE_ITEM            (45)
#define RAW_MEAT_ITEM       (46)
#define COOKED_MEAT_ITEM    (47)
#define EGG_ITEM            (48)
#define STEW_ITEM           (49)
#define CAKE_ITEM           (50)
#define LEATHER_ITEM        (51)
#define WOOL_ITEM           (52)
#define FEATHER_ITEM        (53)
#define BONE_ITEM           (54)
#define STRING_ITEM         (55)
#define SPORE_POWDER_ITEM   (56)
#define BUCKET_ITEM         (57)
#define WATER_BUCKET_ITEM   (58)
#define LAVA_BUCKET_ITEM    (59)
#define SHEARS_ITEM         (60)
#define BOW_ITEM            (61)
#define ARROW_ITEM          (62)
#define FISHING_ROD_ITEM    (63)
#define RAW_FISH_ITEM       (64)
#define COOKED_FISH_ITEM    (65)
#define BED_ITEM            (66)
#define MAP_ITEM            (67)
#define LEATHER_ARMOR_ITEM  (68)
#define IRON_ARMOR_ITEM     (69)
#define GOLD_ARMOR_ITEM     (70)
#define GEM_ARMOR_ITEM      (71)
#define ENCHANT_TABLE_ITEM  (72)

// placeable_on value for building blocks: grass, dirt, sand, flower,
// farmland or wheat
#define PLACE_ON_GROUND (0xfe)

#define ITEMCLASS_MATERIAL   (0)
#define ITEMCLASS_PLACEABLE  (1)
#define ITEMCLASS_FOOD       (2)
#define ITEMCLASS_FURNITURE  (3)
#define ITEMCLASS_POWERGLOVE (4)
#define ITEMCLASS_TOOL       (5)
#define ITEMCLASS_UTILITY    (6) // single, non-stackable: bucket, bow...
#define ITEMCLASS_ARMOR      (7)

struct Item {
    u8 class;
    char *name;
    u8 palette;

    union {
        // ITEMCLASS_PLACEABLE
        struct {
            u8 placed_tile;
            u8 placeable_on[3]; // tile IDs or PLACE_ON_GROUND
        };

        // ITEMCLASS_FOOD
        struct {
            u8 hp_gain;
        };

        // ITEMCLASS_FURNITURE
        struct {
            u8 furniture;
        };
    };
};

struct item_Data {
    u8 type;

    union {
        u16 count;
        u8 chest_id;

        // ITEMCLASS_TOOL
        struct {
            u8 tool_level;
            u8 enchanted;
        };
    };
};

#define ITEM_S(data)\
    (&item_list[(data)->type])

extern const struct Item item_list[ITEM_TYPES];

extern void item_write(struct item_Data *data, u8 palette, u32 x, u32 y);
extern void item_write_name(struct item_Data *data, u8 palette, u32 x, u32 y);
extern void item_draw_icon(struct item_Data *data, u32 x, u32 y, bool black_bg);

// Icon index in items.png. HUD tile = 128 + icon, sprite tile = 256 + icon.
INLINE u16 item_icon(u8 type, u8 tool_level) {
    if(type >= PLANKS_ITEM)
        return type + 22;

    const struct Item *item = &item_list[type];
    return type + (item->class == ITEMCLASS_TOOL) * (2 + tool_level * 5);
}

INLINE bool item_is_resource(u8 type) {
    const struct Item *item = &item_list[type];

    return item->class == ITEMCLASS_MATERIAL  ||
           item->class == ITEMCLASS_PLACEABLE ||
           item->class == ITEMCLASS_FOOD;
}

#endif // MINICRAFT_ITEM
