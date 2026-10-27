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
#ifndef MINICRAFT_ENTITIES
#define MINICRAFT_ENTITIES

#include "main.h"

#include "level.h"
#include "item.h"

#define ENTITY_TYPES (27)

#define ZOMBIE_ENTITY     (0)
#define SLIME_ENTITY      (1)
#define AIR_WIZARD_ENTITY (2)
#define PLAYER_ENTITY     (3)

#define WORKBENCH_ENTITY (4)
#define FURNACE_ENTITY   (5)
#define OVEN_ENTITY      (6)
#define ANVIL_ENTITY     (7)
#define CHEST_ENTITY     (8)
#define LANTERN_ENTITY   (9)

#define ITEM_ENTITY  (10)
#define SPARK_ENTITY (11)

#define TEXT_PARTICLE_ENTITY      (12)
#define SMASH_PARTICLE_ENTITY     (13)

// Mod entities
#define COW_ENTITY         (14)
#define PIG_ENTITY         (15)
#define CHICKEN_ENTITY     (16)
#define SHEEP_ENTITY       (17)
#define WOLF_ENTITY        (18)
#define SKELETON_ENTITY    (19)
#define SPIDER_ENTITY      (20)
#define BLOATER_ENTITY     (21)
#define MAGMA_GOLEM_ENTITY (22)

#define ARROW_ENTITY  (23)
#define XP_ORB_ENTITY (24)

#define BED_ENTITY           (25)
#define ENCHANT_TABLE_ENTITY (26)

#define FIRST_MOD_MOB (COW_ENTITY)
#define LAST_MOD_MOB  (MAGMA_GOLEM_ENTITY)

#define ETICK(name)\
    IWRAM_SECTION\
    static void name(struct Level *level, struct entity_Data *data)

#define EDRAW(name)\
    IWRAM_SECTION\
    static u32 name(struct Level *level, struct entity_Data *data,\
                    u32 used_sprites)

// Mod entities run from ROM: IWRAM has no room left for them
#define ETICK_ROM(name)\
    THUMB\
    static void name(struct Level *level, struct entity_Data *data)

#define EDRAW_ROM(name)\
    THUMB\
    static u32 name(struct Level *level, struct entity_Data *data,\
                    u32 used_sprites)

#define ETOUCH_PLAYER(name)\
    static void name(struct Level *level, struct entity_Data *data,\
                     struct entity_Data *player)

struct Entity {
    void (*tick)(struct Level *level, struct entity_Data *data);

    u32 (*draw)(struct Level *level, struct entity_Data *data,
                u32 used_sprites);

    // entity radius
    u8 xr;
    u8 yr;

    bool is_solid;
    bool hurt_by_tiles;
    void (*touch_player)(struct Level *level, struct entity_Data *data,
                         struct entity_Data *player);
};

extern const struct Entity zombie_entity;

extern const struct Entity zombie_entity;
extern const struct Entity slime_entity;
extern const struct Entity air_wizard_entity;
extern const struct Entity player_entity;

extern const struct Entity workbench_entity;
extern const struct Entity furnace_entity;
extern const struct Entity oven_entity;
extern const struct Entity anvil_entity;
extern const struct Entity chest_entity;
extern const struct Entity lantern_entity;

extern const struct Entity item_entity;
extern const struct Entity spark_entity;

extern const struct Entity text_particle_entity;
extern const struct Entity smash_particle_entity;

extern const struct Entity cow_entity;
extern const struct Entity pig_entity;
extern const struct Entity chicken_entity;
extern const struct Entity sheep_entity;
extern const struct Entity wolf_entity;
extern const struct Entity skeleton_entity;
extern const struct Entity spider_entity;
extern const struct Entity bloater_entity;
extern const struct Entity magma_golem_entity;

extern const struct Entity arrow_entity;
extern const struct Entity xp_orb_entity;

extern const struct Entity bed_entity;
extern const struct Entity enchant_table_entity;

#define ENTITY_S(data)\
    (entity_list[(data)->type])

extern const struct Entity * const entity_list[ENTITY_TYPES];

extern bool entity_move(struct Level *level, struct entity_Data *data,
                        i32 xm, i32 ym);

extern bool entity_move2(struct Level *level, struct entity_Data *data,
                         i32 xm, i32 ym);

INLINE bool entity_intersects(struct entity_Data *data,
                              i32 x0, i32 y0, i32 x1, i32 y1) {
    const struct Entity *entity = ENTITY_S(data);

    return (data->x + entity->xr >= x0) && (data->y + entity->yr >= y0) &&
           (data->x - entity->xr <= x1) && (data->y - entity->yr <= y1);
}

// entity generators

extern void entity_add_zombie(struct Level *level, u16 x, u16 y, u8 lvl);
extern void entity_add_slime(struct Level *level, u16 x, u16 y, u8 lvl);
extern void entity_add_air_wizard(struct Level *level);
extern void entity_add_player(struct Level *level, u8 xt, u8 yt,
                              bool reset_inventory);

extern bool entity_add_furniture(struct Level *level, u8 xt, u8 yt,
                                 struct item_Data *item_data);

extern void entity_add_item(struct Level *level, u16 x, u16 y,
                            u8 item, bool is_tile);
extern void entity_add_spark(struct Level *level, u16 x, u16 y,
                             i8 xv, i8 yv);

extern void entity_add_text_particle(struct Level *level, u16 x, u16 y,
                                     u8 number, u8 palette);
extern void entity_add_smash_particle(struct Level *level, u8 xt, u8 yt);

// mod entity generators
extern void entity_add_mod_mob(struct Level *level, u8 type, u16 x, u16 y);
extern void entity_add_arrow(struct Level *level, u16 x, u16 y,
                             i8 xv, i8 yv, bool from_player);
extern void entity_add_xp_orb(struct Level *level, u16 x, u16 y);

// shears and bones used on mod mobs: return true if the item was used
extern bool mod_mob_shear(struct Level *level, struct entity_Data *data);
extern bool mod_mob_feed_bone(struct Level *level, struct entity_Data *data);

// chooses the mob that 'level_try_spawn' adds
extern void entity_spawn_random_mob(struct Level *level, u16 x, u16 y,
                                    u8 level_index, u8 entity_level);

INLINE bool entity_is_mod_mob(u8 type) {
    return type >= FIRST_MOD_MOB && type <= LAST_MOD_MOB;
}

// any entity whose data is a 'struct mob_Data'
INLINE bool entity_is_mob(u8 type) {
    return type <= PLAYER_ENTITY || entity_is_mod_mob(type);
}

INLINE bool entity_is_hostile(u8 type) {
    return type == ZOMBIE_ENTITY   || type == SLIME_ENTITY  ||
           type == SKELETON_ENTITY || type == SPIDER_ENTITY ||
           type == BLOATER_ENTITY  || type == MAGMA_GOLEM_ENTITY;
}

INLINE bool entity_is_furniture(u8 type) {
    return (type >= WORKBENCH_ENTITY && type <= LANTERN_ENTITY) ||
           type == BED_ENTITY || type == ENCHANT_TABLE_ENTITY;
}

#endif // MINICRAFT_ENTITIES
