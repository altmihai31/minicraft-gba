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
#ifndef MINICRAFT_FURNITURE
#define MINICRAFT_FURNITURE

#include "main.h"

#include "level.h"
#include "inventory.h"
#include "entity.h"
#include "item.h"

#define CHEST_LIMIT (32)
extern struct Inventory chest_inventories[CHEST_LIMIT];
extern u8 chest_count;

extern u8 chest_opened_id;

extern void furniture_take(struct entity_Data *data);

extern void furniture_set_opened_chest(struct entity_Data *data);
extern u8 furniture_new_chest_id(void);

// The original furniture items and entities are contiguous; the mod's
// bed and enchanting table are not.
INLINE u8 furniture_item_to_entity(u8 item) {
    if(item == BED_ITEM)
        return BED_ENTITY;
    if(item == ENCHANT_TABLE_ITEM)
        return ENCHANT_TABLE_ENTITY;
    return WORKBENCH_ENTITY + (item - WORKBENCH_ITEM);
}

INLINE u8 furniture_entity_to_item(u8 type) {
    if(type == BED_ENTITY)
        return BED_ITEM;
    if(type == ENCHANT_TABLE_ENTITY)
        return ENCHANT_TABLE_ITEM;
    return WORKBENCH_ITEM + (type - WORKBENCH_ENTITY);
}

INLINE u16 furniture_sprite(u8 type) {
    if(type == BED_ENTITY)
        return 956;
    if(type == ENCHANT_TABLE_ENTITY)
        return 960;
    return 148 + 4 * (type - WORKBENCH_ENTITY);
}

#endif // MINICRAFT_FURNITURE
