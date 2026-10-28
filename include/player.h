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
#ifndef MINICRAFT_PLAYER
#define MINICRAFT_PLAYER

#include "main.h"

#include "level.h"
#include "inventory.h"
#include "item.h"

extern struct Inventory player_inventory;
extern struct item_Data player_active_item;

extern u8 player_stamina;
extern u8 player_stamina_recharge_delay;

extern u16 player_invulnerable_time;

#define PLAYER_MAX_HP      (10)
#define PLAYER_MAX_STAMINA (10)

#define MAX_ENCHANT_LEVEL (2)

// 0 if the active item is not an enchanted tool, else 1 or 2
INLINE u8 player_active_item_enchant_level(void) {
    if(player_active_item.type < ITEM_TYPES &&
       ITEM_S(&player_active_item)->class == ITEMCLASS_TOOL)
        return player_active_item.enchanted;
    return 0;
}

INLINE bool player_pay_stamina(u8 amount) {
    // enchanted tools cost half (level 1) or a quarter (level 2)
    amount >>= player_active_item_enchant_level();

    if(player_stamina >= amount) {
        player_stamina -= amount;
        return true;
    }
    return false;
}

// tile in front of the player
extern void player_facing_tile(struct entity_Data *data, i32 *xt, i32 *yt);

// mod item actions (src/item-use.c)
extern void item_use(struct Level *level, struct entity_Data *data);
extern bool item_use_furniture(struct Level *level, struct entity_Data *data,
                               struct entity_Data *furniture);

#endif // MINICRAFT_PLAYER
