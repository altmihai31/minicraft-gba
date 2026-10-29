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
#include "entity.h"

#include "mob.h"
#include "tile.h"
#include "item.h"
#include "sound.h"
#include "survival.h"

// All mod mobs share one tick, draw and touch function, driven by the
// 'mod_mobs' table. They run from ROM.

#define KIND_PASSIVE (0) // cow, pig, chicken, sheep
#define KIND_WOLF    (1)
#define KIND_CHASER  (2) // spider
#define KIND_ARCHER  (3) // skeleton
#define KIND_BLOATER (4)
#define KIND_BOSS    (5) // magma golem

struct ModMob {
    u16 sprite; // first OBJ tile of the 5 frames
    u8 palette;
    u8 kind;

    u16 hp;
    u8 hp_per_depth;
    u8 damage; // touch damage, + depth
};

static const struct ModMob mod_mobs[LAST_MOD_MOB - FIRST_MOD_MOB + 1] = {
    [COW_ENTITY         - FIRST_MOD_MOB] = { 744,  8, KIND_PASSIVE, 10,  0, 0 },
    [PIG_ENTITY         - FIRST_MOD_MOB] = { 764,  8, KIND_PASSIVE, 10,  0, 0 },
    [CHICKEN_ENTITY     - FIRST_MOD_MOB] = { 784,  9, KIND_PASSIVE,  4,  0, 0 },
    [SHEEP_ENTITY       - FIRST_MOD_MOB] = { 804,  9, KIND_PASSIVE,  8,  0, 0 },
    [WOLF_ENTITY        - FIRST_MOD_MOB] = { 844, 10, KIND_WOLF,    15,  0, 2 },
    [SKELETON_ENTITY    - FIRST_MOD_MOB] = { 864, 10, KIND_ARCHER,  12,  6, 1 },
    [SPIDER_ENTITY      - FIRST_MOD_MOB] = { 884, 10, KIND_CHASER,  10,  6, 2 },
    [BLOATER_ENTITY     - FIRST_MOD_MOB] = { 904, 11, KIND_BLOATER,  8,  4, 0 },
    [MAGMA_GOLEM_ENTITY - FIRST_MOD_MOB] = { 924, 11, KIND_BOSS,   600,  0, 2 }
};

#define SHEARED_SHEEP_SPRITE (824)

struct modmob_Data {
    i8 xm : 2;
    i8 ym : 2;

    u8 move_flag : 1;
    u8 flag      : 1; // sheep: sheared, wolf: tamed, skeleton: backing off
    u8 depth     : 2; // 0 = surface or sky, 3 = deepest level

    // passive: flee time, wolf: anger/attack delay,
    // skeleton/golem: attack delay, bloater: fuse
    u8 timer;
};

static_assert(
    sizeof(struct modmob_Data) == 2,
    "struct modmob_Data: wrong size"
);

#define MOB_DATA(data)    ((struct mob_Data *) &(data)->data)
#define MODMOB_DATA(data) ((struct modmob_Data *) &MOB_DATA(data)->data)
#define MODMOB_S(data)    (&mod_mobs[(data)->type - FIRST_MOD_MOB])

#define SIGN(x) (((x) > 0) - ((x) < 0))

static inline u8 level_depth(struct Level *level) {
    const i32 index = level - levels;
    return (index >= 3) ? (index == 4) * 3 : 3 - index;
}

THUMB
void entity_add_mod_mob(struct Level *level, u8 type, u16 x, u16 y) {
    u8 entity_id = level_new_entity(level, type);
    if(entity_id >= ENTITY_LIMIT)
        return;

    struct entity_Data *data = &level->entities[entity_id];
    struct mob_Data *mob_data = MOB_DATA(data);
    struct modmob_Data *modmob_data = MODMOB_DATA(data);
    const struct ModMob *mob = MODMOB_S(data);

    data->x = x;
    data->y = y;

    const u8 depth = level_depth(level);
    mob_data->hp = mob->hp + mob->hp_per_depth * depth;
    mob_data->dir = 2;

    modmob_data->depth = depth;

    level_add_entity(level, entity_id);
}

/* ================================================================== */
/*                              Actions                               */
/* ================================================================== */

static void drop_items(struct Level *level, struct entity_Data *data,
                       u8 item, u32 min, u32 extra) {
    const u32 count = min + random(extra + 1);
    for(u32 i = 0; i < count; i++)
        entity_add_item(level, data->x, data->y, item, false);
}

THUMB
static void shoot_at(struct Level *level, struct entity_Data *data,
                     i32 xd, i32 yd) {
    const i32 len = math_sqrt(xd * xd + yd * yd);
    if(len == 0)
        return;

    // velocity in 1/64 pixels per tick
    entity_add_arrow(
        level, data->x, data->y - 2,
        xd * 120 / len, yd * 120 / len,
        false
    );
}

// The golem throws a ring of sparks
THUMB
static void fire_ring(struct Level *level, struct entity_Data *data) {
    for(u32 i = 0; i < 8; i++) {
        const i32 angle = i * 0x2000 + random(0x800);
        entity_add_spark(
            level, data->x, data->y,
            90 * math_cos(angle) / 0x10000,
            90 * math_sin(angle) / 0x10000
        );
    }
}

static inline bool is_hard_tile(u32 tile) {
    switch(tile) {
        case GRASS_TILE:
        case DIRT_TILE:
        case SAND_TILE:
        case LIQUID_TILE:
        case HOLE_TILE:
        case STAIRS_DOWN_TILE:
        case STAIRS_UP_TILE:
        case INFINITE_FALL_TILE:
        case CLOUD_TILE:
        case HARD_ROCK_TILE:
        case IRON_ORE_TILE:
        case GOLD_ORE_TILE:
        case GEM_ORE_TILE:
        case CLOUD_CACTUS_TILE:
        case OBSIDIAN_TILE:
        case SPAWNER_TILE:
            return true;
    }
    return false;
}

#define EXPLOSION_RANGE (24)

// The bloater destroys the soft tiles around it and hurts everything
// close enough, then disappears.
THUMB
static void explode(struct Level *level, struct entity_Data *data) {
    const struct modmob_Data *modmob_data = MODMOB_DATA(data);
    const i32 xt0 = data->x >> 4;
    const i32 yt0 = data->y >> 4;

    for(i32 yt = yt0 - 1; yt <= yt0 + 1; yt++) {
        for(i32 xt = xt0 - 1; xt <= xt0 + 1; xt++) {
            if(xt < 0 || xt >= LEVEL_W || yt < 0 || yt >= LEVEL_H)
                continue;

            if(is_hard_tile(LEVEL_GET_TILE(level, xt, yt)))
                continue;

            LEVEL_SET_TILE(level, xt, yt, DIRT_TILE, 0);
            entity_add_smash_particle(level, xt, yt);
        }
    }

    const i32 x0 = data->x - EXPLOSION_RANGE;
    const i32 y0 = data->y - EXPLOSION_RANGE;
    const i32 x1 = data->x + EXPLOSION_RANGE;
    const i32 y1 = data->y + EXPLOSION_RANGE;

    for(u32 i = 0; i < ENTITY_LIMIT; i++) {
        struct entity_Data *e_data = &level->entities[i];
        if(e_data == data || !entity_is_mob(e_data->type))
            continue;

        if(entity_intersects(e_data, x0, y0, x1, y1)) {
            const i32 xd = e_data->x - data->x;
            const i32 yd = e_data->y - data->y;
            const u8 dir = (math_abs(xd) > math_abs(yd)) ?
                           (xd < 0 ? 1 : 3) : (yd < 0 ? 0 : 2);

            mob_hurt(level, e_data, 5 + modmob_data->depth, dir);
        }
    }

    drop_items(level, data, SPORE_POWDER_ITEM, 0, 2);
    SOUND_PLAY(sound_monster_hurt);

    data->should_remove = true;
}

/* ================================================================== */
/*                                AI                                  */
/* ================================================================== */

// pick a new random direction, or stop
static inline void wander(struct modmob_Data *modmob_data) {
    modmob_data->xm = random(2) * (random(3) - 1);
    modmob_data->ym = random(2) * (random(3) - 1);
}

#define WOLF_SIGHT (6 * 16)

// nearest hostile mob for a tamed wolf
static struct entity_Data *find_prey(struct Level *level,
                                     struct entity_Data *data) {
    struct entity_Data *prey = NULL;
    u32 best = WOLF_SIGHT * WOLF_SIGHT;

    for(u32 i = 1; i < ENTITY_LIMIT; i++) {
        struct entity_Data *e_data = &level->entities[i];
        if(!entity_is_hostile(e_data->type))
            continue;

        const i32 xd = e_data->x - data->x;
        const i32 yd = e_data->y - data->y;
        const u32 dist = xd * xd + yd * yd;
        if(dist < best) {
            best = dist;
            prey = e_data;
        }
    }
    return prey;
}

ETICK_ROM(mod_mob_tick) {
    struct mob_Data    *mob_data    = MOB_DATA(data);
    struct modmob_Data *modmob_data = MODMOB_DATA(data);
    const struct ModMob *mob = MODMOB_S(data);

    mob_tick(level, data);
    if(data->should_remove)
        return;

    struct entity_Data *player = &level->entities[0];
    const bool has_player = (player->type < ENTITY_TYPES);

    const i32 xd = has_player ? player->x - data->x : 0;
    const i32 yd = has_player ? player->y - data->y : 0;
    const u32 dist = has_player ? xd * xd + yd * yd : (u32) -1;

    const bool tamed = (data->type == WOLF_ENTITY && modmob_data->flag);

    // despawn when far away, like zombies (not the boss or tamed wolves)
    if(has_player && dist >= DESPAWN_DISTANCE &&
       mob->kind != KIND_BOSS && !tamed) {
        data->should_remove = true;
        return;
    }

    // just hit (mob_hurt sets 10, mob_tick decremented it)
    const bool just_hurt = (mob_data->hurt_time == 9);

    // 'chase' moves towards (xd, yd), 'flee' away from it
    bool chase = false;
    bool flee  = false;
    bool fast  = false;
    bool hold  = false; // stand still (skeleton aiming)
    i32 txd = xd, tyd = yd; // target offset

    switch(mob->kind) {
        case KIND_PASSIVE:
            if(just_hurt)
                modmob_data->timer = 60;

            if(modmob_data->timer > 0) {
                modmob_data->timer--;
                flee = fast = true;
            }

            // chickens lay eggs, sheep regrow their wool
            if(data->type == CHICKEN_ENTITY && random(40 * 60) == 0)
                entity_add_item(level, data->x, data->y, EGG_ITEM, false);
            if(data->type == SHEEP_ENTITY && modmob_data->flag &&
               random(60 * 60) == 0)
                modmob_data->flag = 0;
            break;

        case KIND_WOLF:
            if(modmob_data->timer > 0)
                modmob_data->timer--;

            if(tamed) {
                struct entity_Data *prey = find_prey(level, data);
                if(prey) {
                    txd = prey->x - data->x;
                    tyd = prey->y - data->y;
                    chase = fast = true;

                    if(txd * txd + tyd * tyd < 14 * 14 &&
                       modmob_data->timer == 0) {
                        mob_hurt(level, prey, 3, mob_data->dir);
                        modmob_data->timer = 30;
                    }
                } else if(dist > 40 * 40) {
                    // follow the player
                    chase = true;
                    fast = (dist > 96 * 96);
                }
            } else {
                // an untamed wolf gets angry when hit
                if(just_hurt)
                    modmob_data->timer = 240;
                if(modmob_data->timer > 0 && dist < 96 * 96)
                    chase = fast = true;
            }
            break;

        case KIND_CHASER:
            if(dist < 64 * 64)
                chase = fast = true;
            break;

        case KIND_ARCHER:
            if(modmob_data->timer > 0)
                modmob_data->timer--;

            // back off when the player comes within 2 tiles, until 3
            // tiles away ('flag': no flip-flop at the edge)
            if(dist < 32 * 32)
                modmob_data->flag = 1;
            else if(dist > 48 * 48)
                modmob_data->flag = 0;

            if(dist < 80 * 80) {
                if(modmob_data->flag) {
                    // walk away, facing where it walks
                    flee = true;
                } else {
                    // stand still and face the player: turning to aim
                    // while walking flipped the sprite every tick
                    hold = true;
                    modmob_data->xm = 0;
                    modmob_data->ym = 0;
                    mob_data->dir = (math_abs(xd) > math_abs(yd)) ?
                                    (xd < 0 ? 1 : 3) : (yd < 0 ? 0 : 2);
                }

                if(modmob_data->timer == 0) {
                    shoot_at(level, data, xd, yd);
                    modmob_data->timer = 120 - modmob_data->depth * 10;
                }
            }
            break;

        case KIND_BLOATER:
            if(dist < 20 * 20) {
                modmob_data->timer++;
                if(modmob_data->timer >= 45) {
                    explode(level, data);
                    return;
                }
            } else if(modmob_data->timer > 0) {
                modmob_data->timer--;
            }

            if(dist < 64 * 64)
                chase = true;
            break;

        case KIND_BOSS:
            if(modmob_data->timer > 0)
                modmob_data->timer--;

            if(dist < 160 * 160) {
                chase = true;

                if(modmob_data->timer == 0 && dist < 128 * 128) {
                    fire_ring(level, data);
                    modmob_data->timer = 150;
                }
            }
            break;
    }

    if(chase || flee) {
        const i32 s = flee ? -1 : 1;
        modmob_data->xm = SIGN(txd) * s;
        modmob_data->ym = SIGN(tyd) * s;
    }

    modmob_data->move_flag ^= 1;
    const u32 step = fast || modmob_data->move_flag;

    const bool moved = mob_move(
        level, data,
        modmob_data->xm * step,
        modmob_data->ym * step
    );

    if(!chase && !flee && !hold && (!moved || random(100) == 0))
        wander(modmob_data);
}

EDRAW_ROM(mod_mob_draw) {
    struct mob_Data    *mob_data    = MOB_DATA(data);
    struct modmob_Data *modmob_data = MODMOB_DATA(data);
    const struct ModMob *mob = MODMOB_S(data);

    const u8 dir = mob_data->dir;
    const u8 walk_dist = mob_data->walk_dist;

    u16 sprite = mob->sprite;
    if(data->type == SHEEP_ENTITY && modmob_data->flag)
        sprite = SHEARED_SHEEP_SPRITE;

    // frames: front, back, side, walk A, walk B (like the zombie)
    sprite += (dir == 0) * 4 + (dir & 1) * 8;
    sprite += (dir & 1) * (
        ((walk_dist >> 3) & 1) * (4 + ((walk_dist >> 4) & 1) * 4)
    );

    // flash white when hurt, and while a bloater's fuse burns
    bool flash = (mob_data->hurt_time > 0);
    if(data->type == BLOATER_ENTITY && modmob_data->timer > 0)
        flash |= (modmob_data->timer >> 2) & 1;

    sprite_config(used_sprites, &(struct Sprite) {
        .x = data->x - 8 - level_x_offset,
        .y = data->y - 11 - level_y_offset,

        .priority = 2,

        .size = SPRITE_SIZE_16x16,
        .flip = ((dir & 1) == 0) * ((walk_dist >> 3) & 1) + (dir == 1),

        .tile = sprite,
        .palette = flash ? 5 : mob->palette
    });
    return 1;
}

ETOUCH_PLAYER(mod_mob_touch_player) {
    struct mob_Data    *mob_data    = MOB_DATA(data);
    struct modmob_Data *modmob_data = MODMOB_DATA(data);
    const struct ModMob *mob = MODMOB_S(data);

    if(mob->damage == 0)
        return;

    // wolves only bite when angry
    if(data->type == WOLF_ENTITY &&
       (modmob_data->flag || modmob_data->timer == 0))
        return;

    mob_hurt(level, player, mob->damage + modmob_data->depth, mob_data->dir);
}

#define GENERATE_STRUCT(name)\
    const struct Entity name = {\
        .tick = mod_mob_tick,\
        .draw = mod_mob_draw,\
\
        .xr = 4,\
        .yr = 3,\
\
        .is_solid = true,\
        .hurt_by_tiles = true,\
        .touch_player = mod_mob_touch_player\
    }

GENERATE_STRUCT(cow_entity);
GENERATE_STRUCT(pig_entity);
GENERATE_STRUCT(chicken_entity);
GENERATE_STRUCT(sheep_entity);
GENERATE_STRUCT(wolf_entity);
GENERATE_STRUCT(skeleton_entity);
GENERATE_STRUCT(spider_entity);
GENERATE_STRUCT(bloater_entity);
GENERATE_STRUCT(magma_golem_entity);

/* ================================================================== */
/*                       Player interactions                          */
/* ================================================================== */

THUMB
bool mod_mob_shear(struct Level *level, struct entity_Data *data) {
    struct modmob_Data *modmob_data = MODMOB_DATA(data);
    if(data->type != SHEEP_ENTITY || modmob_data->flag)
        return false;

    modmob_data->flag = 1;
    drop_items(level, data, WOOL_ITEM, 1, 2);
    return true;
}

// returns true if the bone was used
THUMB
bool mod_mob_feed_bone(struct Level *level, struct entity_Data *data) {
    struct modmob_Data *modmob_data = MODMOB_DATA(data);
    if(data->type != WOLF_ENTITY || modmob_data->flag)
        return false;

    if(random(3) == 0) {
        modmob_data->flag = 1;
        modmob_data->timer = 0;

        // show a '+' over the wolf
        entity_add_text_particle(level, data->x, data->y, 1, 2);
    }
    return true;
}

/* ================================================================== */
/*                         Death and spawning                         */
/* ================================================================== */

THUMB
void mob_mod_die(struct Level *level, struct entity_Data *data) {
    struct modmob_Data *modmob_data = MODMOB_DATA(data);

    switch(data->type) {
        case COW_ENTITY:
            drop_items(level, data, RAW_MEAT_ITEM, 1, 1);
            drop_items(level, data, LEATHER_ITEM, 0, 2);
            break;

        case PIG_ENTITY:
            drop_items(level, data, RAW_MEAT_ITEM, 1, 2);
            break;

        case CHICKEN_ENTITY:
            drop_items(level, data, FEATHER_ITEM, 0, 2);
            drop_items(level, data, RAW_MEAT_ITEM, 1, 0);
            break;

        case SHEEP_ENTITY:
            drop_items(level, data, RAW_MEAT_ITEM, 1, 0);
            if(!modmob_data->flag)
                drop_items(level, data, WOOL_ITEM, 1, 0);
            break;

        case WOLF_ENTITY:
            drop_items(level, data, BONE_ITEM, 0, 1);
            break;

        case SKELETON_ENTITY:
            drop_items(level, data, BONE_ITEM, 0, 2);
            drop_items(level, data, ARROW_ITEM, 0, 2);
            break;

        case SPIDER_ENTITY:
            drop_items(level, data, STRING_ITEM, 0, 2);
            break;

        case BLOATER_ENTITY:
            drop_items(level, data, SPORE_POWDER_ITEM, 0, 2);
            break;

        case MAGMA_GOLEM_ENTITY:
            drop_items(level, data, GEM_ITEM, 5, 3);
            drop_items(level, data, OBSIDIAN_ITEM, 2, 2);
            break;
    }

    if(entity_is_hostile(data->type)) {
        const u32 orbs = (data->type == MAGMA_GOLEM_ENTITY) ? 20 :
                         1 + random(2);
        for(u32 i = 0; i < orbs; i++)
            entity_add_xp_orb(level, data->x, data->y);
    }

    struct entity_Data *player = &level->entities[0];
    if(player->type < ENTITY_TYPES) {
        if(data->type == MAGMA_GOLEM_ENTITY)
            score += 2000;
        else if(entity_is_hostile(data->type))
            score += 50 * (1 + modmob_data->depth);
        else
            score += 10;
    }
}

// Replaces the zombie/slime coin toss of the original game: some of the
// spawns become mod mobs.
THUMB
void entity_spawn_random_mob(struct Level *level, u16 x, u16 y,
                             u8 level_index, u8 entity_level) {
    u8 type = -1;

    if(level_index == 3 && survival_is_night()) {
        // surface at night: monsters only
        switch(random(4)) {
            case 0: type = SKELETON_ENTITY; break;
            case 1: type = SPIDER_ENTITY;   break;
            default:
                entity_add_zombie(level, x, y, entity_level);
                return;
        }
    } else if(level_index == 3) {
        // surface: animals, and a few new monsters
        switch(random(10)) {
            case 0: type = COW_ENTITY;     break;
            case 1: type = PIG_ENTITY;     break;
            case 2: type = CHICKEN_ENTITY; break;
            case 3: type = SHEEP_ENTITY;   break;
            case 4:
                type = random(3) ? SHEEP_ENTITY : WOLF_ENTITY;
                break;
            case 5: type = SKELETON_ENTITY; break;
            case 6: type = SPIDER_ENTITY;   break;
        }
    } else if(level_index < 3) {
        // underground
        switch(random(8)) {
            case 0: type = SKELETON_ENTITY; break;
            case 1: type = SPIDER_ENTITY;   break;
            case 2: type = BLOATER_ENTITY;  break;
        }
    }

    if(type != (u8) -1) {
        entity_add_mod_mob(level, type, x, y);
        return;
    }

    if(random(2))
        entity_add_slime(level, x, y, entity_level);
    else
        entity_add_zombie(level, x, y, entity_level);
}
