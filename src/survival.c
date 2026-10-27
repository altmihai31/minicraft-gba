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
#include "survival.h"

#include "entity.h"
#include "mob.h"
#include "player.h"
#include "item.h"
#include "screen.h"

// lose one hunger point every 40 seconds
#define HUNGER_DRAIN_TICKS (40 * 60)

// regenerate one HP every 10 seconds while not starving
#define REGEN_TICKS (10 * 60)

SBSS_SECTION u8 player_hunger;
SBSS_SECTION u16 player_hunger_timer;

SBSS_SECTION u8 player_armor;
SBSS_SECTION u16 player_xp;

SBSS_SECTION u8 spawn_level;
SBSS_SECTION u8 spawn_xt;
SBSS_SECTION u8 spawn_yt;

// not saved: fishing and sliding just stop when loading
SBSS_SECTION static u8 fishing_timer;

SBSS_SECTION static i8 slide_xm;
SBSS_SECTION static i8 slide_ym;
SBSS_SECTION static u8 slide_time;

THUMB
void survival_reset(bool reset_inventory) {
    player_hunger = MAX_HUNGER;
    player_hunger_timer = 0;

    fishing_timer = 0;
    slide_time = 0;

    if(reset_inventory) {
        player_armor = -1;
        player_xp = 0;
    }
}

THUMB
void survival_player_tick(struct Level *level, struct entity_Data *data) {
    struct mob_Data *mob_data = (struct mob_Data *) &data->data;

    player_hunger_timer++;
    if(player_hunger_timer >= HUNGER_DRAIN_TICKS) {
        player_hunger_timer = 0;

        if(player_hunger > 0)
            player_hunger--;
    }

    if(player_hunger > 0 && mob_data->hp < PLAYER_MAX_HP &&
       gametime % REGEN_TICKS == 0)
        mob_data->hp++;

    // fishing: switching item cancels it
    if(fishing_timer > 0) {
        if(player_active_item.type != FISHING_ROD_ITEM) {
            fishing_timer = 0;
        } else if(--fishing_timer == 0 && random(10) >= 3) {
            entity_add_item(level, data->x, data->y, RAW_FISH_ITEM, false);
            entity_add_text_particle(level, data->x, data->y - 8, 1, 2);
        }
    }
}

THUMB
void survival_start_fishing(void) {
    if(fishing_timer == 0)
        fishing_timer = 60 + random(4 * 60);
}

#define SLIDE_TICKS (16)

THUMB
void survival_ice_slide(i32 *xm, i32 *ym) {
    if(*xm != 0 || *ym != 0) {
        slide_xm = *xm;
        slide_ym = *ym;
        slide_time = SLIDE_TICKS;
    } else if(slide_time > 0) {
        slide_time--;
        *xm = slide_xm;
        *ym = slide_ym;
    }
}

// chance (%) of fully blocking a hit, and the HUD points shown
static const u8 armor_block_chance[4] = { 20, 35, 50, 65 };
static const u8 armor_points[4]       = {  2,  4,  5,  7 };

static inline i32 armor_index(void) {
    if(player_armor >= LEATHER_ARMOR_ITEM && player_armor <= GEM_ARMOR_ITEM)
        return player_armor - LEATHER_ARMOR_ITEM;
    return -1;
}

THUMB
bool survival_armor_blocks(void) {
    const i32 index = armor_index();
    if(index < 0)
        return false;

    return random(100) < armor_block_chance[index];
}

THUMB
void survival_draw_status_bar(void) {
    const i32 index = armor_index();
    const u32 points = (index < 0) ? 0 : armor_points[index];

    for(u32 i = 0; i < 10; i++) {
        BG3_TILEMAP[(10 + i) + 18 * 32] = (104 + (player_hunger <= i)) | 10 << 12;
        BG3_TILEMAP[(10 + i) + 19 * 32] = (106 + (points <= i))        | 10 << 12;
    }

    // numbers are left-aligned: clear the old digits first
    screen_write("XP      ", 0, 21, 19);
    SCREEN_WRITE_NUMBER(player_xp, 10, 5, false, 3, 24, 19);
}
