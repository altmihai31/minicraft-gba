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
#include "crafting.h"

#include "inventory.h"
#include "item.h"
#include "furniture.h"

const struct crafting_Recipe workbench_recipes[WORKBENCH_RECIPES] = {
    // Furniture

    // Lantern
    {
        .required.items = { WOOD_ITEM, SLIME_ITEM, GLASS_ITEM },
        .required.count = { 5,         10,         4          },

        .result = LANTERN_ITEM
    },

    // Oven
    {
        .required.items = { STONE_ITEM },
        .required.count = { 15 },

        .result = OVEN_ITEM
    },

    // Furnace
    {
        .required.items = { STONE_ITEM },
        .required.count = { 20 },

        .result = FURNACE_ITEM
    },

    // Workbench
    {
        .required.items = { WOOD_ITEM },
        .required.count = { 20 },

        .result = WORKBENCH_ITEM
    },

    // Chest
    {
        .required.items = { WOOD_ITEM },
        .required.count = { 20 },

        .result = CHEST_ITEM
    },

    // Anvil
    {
        .required.items = { IRON_INGOT_ITEM },
        .required.count = { 5 },

        .result = ANVIL_ITEM
    },

    // Wood Tools

    // Wood Sword
    {
        .required.items = { WOOD_ITEM },
        .required.count = { 5 },

        .result = SWORD_ITEM,
        .tool_level = 0
    },

    // Wood Axe
    {
        .required.items = { WOOD_ITEM },
        .required.count = { 5 },

        .result = AXE_ITEM,
        .tool_level = 0
    },

    // Wood Hoe
    {
        .required.items = { WOOD_ITEM },
        .required.count = { 5 },

        .result = HOE_ITEM,
        .tool_level = 0
    },

    // Wood Pick
    {
        .required.items = { WOOD_ITEM },
        .required.count = { 5 },

        .result = PICK_ITEM,
        .tool_level = 0
    },

    // Wood Shovel
    {
        .required.items = { WOOD_ITEM },
        .required.count = { 5 },

        .result = SHOVEL_ITEM,
        .tool_level = 0
    },

    // Stone Tools

    // Stone Sword
    {
        .required.items = { WOOD_ITEM, STONE_ITEM },
        .required.count = { 5,         5          },

        .result = SWORD_ITEM,
        .tool_level = 1
    },

    // Stone Axe
    {
        .required.items = { WOOD_ITEM, STONE_ITEM },
        .required.count = { 5,         5          },

        .result = AXE_ITEM,
        .tool_level = 1
    },

    // Stone Hoe
    {
        .required.items = { WOOD_ITEM, STONE_ITEM },
        .required.count = { 5,         5          },

        .result = HOE_ITEM,
        .tool_level = 1
    },

    // Stone Pick
    {
        .required.items = { WOOD_ITEM, STONE_ITEM },
        .required.count = { 5,         5          },

        .result = PICK_ITEM,
        .tool_level = 1
    },

    // Stone Shovel
    {
        .required.items = { WOOD_ITEM, STONE_ITEM },
        .required.count = { 5,         5          },

        .result = SHOVEL_ITEM,
        .tool_level = 1
    },

    // Mod recipes

    // Planks
    {
        .required.items = { WOOD_ITEM },
        .required.count = { 1 },

        .result = PLANKS_ITEM,
        .result_count = 2
    },

    // Door
    {
        .required.items = { PLANKS_ITEM },
        .required.count = { 6 },

        .result = DOOR_ITEM
    },

    // Torch
    {
        .required.items = { WOOD_ITEM, COAL_ITEM },
        .required.count = { 1,         1         },

        .result = TORCH_ITEM,
        .result_count = 4
    },

    // Paper
    {
        .required.items = { SUGAR_CANE_ITEM },
        .required.count = { 3 },

        .result = PAPER_ITEM,
        .result_count = 3
    },

    // Stone Bricks
    {
        .required.items = { STONE_ITEM },
        .required.count = { 4 },

        .result = STONE_BRICKS_ITEM,
        .result_count = 4
    },

    // Bow
    {
        .required.items = { WOOD_ITEM, STRING_ITEM },
        .required.count = { 5,         3           },

        .result = BOW_ITEM
    },

    // Arrows
    {
        .required.items = { WOOD_ITEM, STONE_ITEM, FEATHER_ITEM },
        .required.count = { 1,         1,          1            },

        .result = ARROW_ITEM,
        .result_count = 4
    },

    // Fishing Rod
    {
        .required.items = { WOOD_ITEM, STRING_ITEM },
        .required.count = { 3,         2           },

        .result = FISHING_ROD_ITEM
    },

    // Bed
    {
        .required.items = { WOOD_ITEM, WOOL_ITEM },
        .required.count = { 5,         3         },

        .result = BED_ITEM
    },

    // Map
    {
        .required.items = { PAPER_ITEM },
        .required.count = { 8 },

        .result = MAP_ITEM
    },

    // Leather Armor
    {
        .required.items = { LEATHER_ITEM },
        .required.count = { 8 },

        .result = LEATHER_ARMOR_ITEM
    }
};

const struct crafting_Recipe furnace_recipes[FURNACE_RECIPES] = {
    // Iron Ingot
    {
        .required.items = { IRON_ORE_ITEM, COAL_ITEM },
        .required.count = { 4,             1         },

        .result = IRON_INGOT_ITEM
    },

    // Gold Ingot
    {
        .required.items = { GOLD_ORE_ITEM, COAL_ITEM },
        .required.count = { 4,             1         },

        .result = GOLD_INGOT_ITEM
    },

    // Glass
    {
        .required.items = { SAND_ITEM, COAL_ITEM },
        .required.count = { 4,         1         },

        .result = GLASS_ITEM
    },

    // Brick
    {
        .required.items = { CLAY_ITEM, COAL_ITEM },
        .required.count = { 1,         1         },

        .result = BRICK_ITEM
    }
};

const struct crafting_Recipe oven_recipes[OVEN_RECIPES] = {
    // Bread
    {
        .required.items = { WHEAT_ITEM },
        .required.count = { 4 },

        .result = BREAD_ITEM
    },

    // Cooked Meat
    {
        .required.items = { RAW_MEAT_ITEM, COAL_ITEM },
        .required.count = { 1,             1         },

        .result = COOKED_MEAT_ITEM
    },

    // Cooked Fish
    {
        .required.items = { RAW_FISH_ITEM, COAL_ITEM },
        .required.count = { 1,             1         },

        .result = COOKED_FISH_ITEM
    },

    // Mushroom Stew
    {
        .required.items = { MUSHROOM_ITEM, WOOD_ITEM },
        .required.count = { 2,             1         },

        .result = STEW_ITEM
    },

    // Cake
    {
        .required.items = { WHEAT_ITEM, EGG_ITEM, SUGAR_CANE_ITEM },
        .required.count = { 3,          1,        1               },

        .result = CAKE_ITEM
    }
};

const struct crafting_Recipe anvil_recipes[ANVIL_RECIPES] = {
    // Iron Tools

    // Iron Sword
    {
        .required.items = { WOOD_ITEM, IRON_INGOT_ITEM },
        .required.count = { 5,         5               },

        .result = SWORD_ITEM,
        .tool_level = 2
    },

    // Iron Axe
    {
        .required.items = { WOOD_ITEM, IRON_INGOT_ITEM },
        .required.count = { 5,         5               },

        .result = AXE_ITEM,
        .tool_level = 2
    },

    // Iron Hoe
    {
        .required.items = { WOOD_ITEM, IRON_INGOT_ITEM },
        .required.count = { 5,         5               },

        .result = HOE_ITEM,
        .tool_level = 2
    },

    // Iron Pick
    {
        .required.items = { WOOD_ITEM, IRON_INGOT_ITEM },
        .required.count = { 5,         5               },

        .result = PICK_ITEM,
        .tool_level = 2
    },

    // Iron Shovel
    {
        .required.items = { WOOD_ITEM, IRON_INGOT_ITEM },
        .required.count = { 5,         5               },

        .result = SHOVEL_ITEM,
        .tool_level = 2
    },

    // Gold Tools

    // Gold Sword
    {
        .required.items = { WOOD_ITEM, GOLD_INGOT_ITEM },
        .required.count = { 5,         5               },

        .result = SWORD_ITEM,
        .tool_level = 3
    },

    // Gold Axe
    {
        .required.items = { WOOD_ITEM, GOLD_INGOT_ITEM },
        .required.count = { 5,         5               },

        .result = AXE_ITEM,
        .tool_level = 3
    },

    // Gold Hoe
    {
        .required.items = { WOOD_ITEM, GOLD_INGOT_ITEM },
        .required.count = { 5,         5               },

        .result = HOE_ITEM,
        .tool_level = 3
    },

    // Gold Pick
    {
        .required.items = { WOOD_ITEM, GOLD_INGOT_ITEM },
        .required.count = { 5,         5               },

        .result = PICK_ITEM,
        .tool_level = 3
    },

    // Gold Shovel
    {
        .required.items = { WOOD_ITEM, GOLD_INGOT_ITEM },
        .required.count = { 5,         5               },

        .result = SHOVEL_ITEM,
        .tool_level = 3
    },

    // Gem Tools

    // Gem Sword
    {
        .required.items = { WOOD_ITEM, GEM_ITEM },
        .required.count = { 5,         50       },

        .result = SWORD_ITEM,
        .tool_level = 4
    },

    // Gem Axe
    {
        .required.items = { WOOD_ITEM, GEM_ITEM },
        .required.count = { 5,         50       },

        .result = AXE_ITEM,
        .tool_level = 4
    },

    // Gem Hoe
    {
        .required.items = { WOOD_ITEM, GEM_ITEM },
        .required.count = { 5,         50       },

        .result = HOE_ITEM,
        .tool_level = 4
    },

    // Gem Pick
    {
        .required.items = { WOOD_ITEM, GEM_ITEM },
        .required.count = { 5,         50       },

        .result = PICK_ITEM,
        .tool_level = 4
    },

    // Gem Shovel
    {
        .required.items = { WOOD_ITEM, GEM_ITEM },
        .required.count = { 5,         50       },

        .result = SHOVEL_ITEM,
        .tool_level = 4
    },

    // Mod recipes

    // Bucket
    {
        .required.items = { IRON_INGOT_ITEM },
        .required.count = { 3 },

        .result = BUCKET_ITEM
    },

    // Shears
    {
        .required.items = { IRON_INGOT_ITEM },
        .required.count = { 2 },

        .result = SHEARS_ITEM
    },

    // Iron Armor
    {
        .required.items = { IRON_INGOT_ITEM },
        .required.count = { 8 },

        .result = IRON_ARMOR_ITEM
    },

    // Gold Armor
    {
        .required.items = { GOLD_INGOT_ITEM },
        .required.count = { 8 },

        .result = GOLD_ARMOR_ITEM
    },

    // Gem Armor
    {
        .required.items = { GEM_ITEM },
        .required.count = { 15 },

        .result = GEM_ARMOR_ITEM
    },

    // Enchanting Table
    {
        .required.items = { OBSIDIAN_ITEM, GEM_ITEM, PAPER_ITEM },
        .required.count = { 4,             2,        6          },

        .result = ENCHANT_TABLE_ITEM
    }
};

u8 crafting_current_recipes_size;
const struct crafting_Recipe *crafting_current_recipes;

bool crafting_craft(struct Inventory *inventory,
                    const struct crafting_Recipe *recipe) {
    // add item to the inventory
    if(item_is_resource(recipe->result)) {
        const u16 count = recipe->result_count ? recipe->result_count : 1;
        if(!inventory_add_resource(inventory, recipe->result, count, 0))
            return false;
    } else {
        struct item_Data to_add = { .type = recipe->result };

        if(ITEM_S(&to_add)->class == ITEMCLASS_TOOL)
            to_add.tool_level = recipe->tool_level;

        if(to_add.type == CHEST_ITEM) {
            to_add.chest_id = furniture_new_chest_id();

            if(to_add.chest_id >= CHEST_LIMIT)
                return false;
        }

        if(!inventory_add(inventory, &to_add, 0))
            return false;
    }

    // remove the required items
    // (assume that the inventory contains them)
    for(u32 i = 0; i < CRAFTING_MAX_REQUIRED; i++) {
        const u8 item_type = recipe->required.items[i];
        const u8 count     = recipe->required.count[i];

        if(count == 0)
            continue;

        inventory_remove_resource(inventory, item_type, count);
    }

    return true;
}
