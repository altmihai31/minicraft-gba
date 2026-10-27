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
#include "survival.h"
#include "sound.h"

/* ================================================================== */
/*                               Arrow                                */
/* ================================================================== */

#define ARROW_SPRITE_H (944) // points right
#define ARROW_SPRITE_V (948) // points up

struct arrow_Data {
    u16 time;

    // 64 xx = 1 x
    // 64 yy = 1 y
    i16 xx;
    i16 yy;

    // velocity
    i8 xv;
    i8 yv;
};

static_assert(sizeof(struct arrow_Data) == 8, "struct arrow_Data: wrong size");

// arrows are not solid: 'solid_id' stores who shot them
#define FROM_PLAYER(data) ((data)->solid_id)

THUMB
void entity_add_arrow(struct Level *level, u16 x, u16 y,
                      i8 xv, i8 yv, bool from_player) {
    u8 entity_id = level_new_entity(level, ARROW_ENTITY);
    if(entity_id >= ENTITY_LIMIT)
        return;

    struct entity_Data *data = &level->entities[entity_id];
    struct arrow_Data *arrow_data = (struct arrow_Data *) &data->data;

    data->x = x;
    data->y = y;

    arrow_data->xv = xv;
    arrow_data->yv = yv;
    arrow_data->time = 60;

    level_add_entity(level, entity_id);
    FROM_PLAYER(data) = from_player;
}

static inline u8 arrow_dir(struct arrow_Data *arrow_data) {
    if(math_abs(arrow_data->xv) > math_abs(arrow_data->yv))
        return (arrow_data->xv < 0) ? 1 : 3;
    return (arrow_data->yv < 0) ? 0 : 2;
}

// returns true if the arrow hit something
static inline bool arrow_hit(struct Level *level, struct entity_Data *data,
                             struct arrow_Data *arrow_data) {
    const u32 xt = data->x >> 4;
    const u32 yt = data->y >> 4;
    if(xt >= LEVEL_W || yt >= LEVEL_H)
        return true;

    // walls stop arrows, water and holes don't
    const struct Tile *tile = LEVEL_GET_TILE_S(level, xt, yt);
    if(tile->is_solid && tile->may_pass == (u8) -1)
        return true;

    const u8 *solid_entities = level_solid_entities[xt + yt * LEVEL_W];
    for(u32 i = 0; i < SOLID_ENTITIES_IN_TILE; i++) {
        if(solid_entities[i] >= ENTITY_LIMIT)
            continue;

        struct entity_Data *e_data = &level->entities[solid_entities[i]];
        if(!entity_is_mob(e_data->type))
            continue;

        // player arrows hit mobs, skeleton arrows hit the player
        const bool is_player = (e_data->type == PLAYER_ENTITY);
        if(is_player == FROM_PLAYER(data))
            continue;

        if(entity_intersects(e_data, data->x, data->y, data->x, data->y)) {
            // skeleton arrows hurt more on the two deepest levels
            const u8 damage = FROM_PLAYER(data) ? 3 + random(3)
                                                : 1 + (level < &levels[2]);
            mob_hurt(level, e_data, damage, arrow_dir(arrow_data));
            return true;
        }
    }
    return false;
}

ETICK_ROM(arrow_tick) {
    struct arrow_Data *arrow_data = (struct arrow_Data *) &data->data;

    arrow_data->time--;
    if(arrow_data->time == 0) {
        data->should_remove = true;
        return;
    }

    arrow_data->xx += arrow_data->xv;
    arrow_data->yy += arrow_data->yv;

    data->x += (arrow_data->xx / 64);
    data->y += (arrow_data->yy / 64);

    arrow_data->xx %= 64;
    arrow_data->yy %= 64;

    if(arrow_hit(level, data, arrow_data))
        data->should_remove = true;
}

EDRAW_ROM(arrow_draw) {
    struct arrow_Data *arrow_data = (struct arrow_Data *) &data->data;
    const u8 dir = arrow_dir(arrow_data);

    sprite_config(used_sprites, &(struct Sprite) {
        .x = data->x - 8 - level_x_offset,
        .y = data->y - 8 - level_y_offset,

        .priority = 2,

        .size = SPRITE_SIZE_16x16,
        .flip = (dir == 1) * 1 + (dir == 2) * 2,

        .tile = (dir & 1) ? ARROW_SPRITE_H : ARROW_SPRITE_V,
        .palette = 9
    });
    return 1;
}

const struct Entity arrow_entity = {
    .tick = arrow_tick,
    .draw = arrow_draw,

    .xr = 2,
    .yr = 2
};

/* ================================================================== */
/*                              XP Orb                                */
/* ================================================================== */

#define XP_ORB_SPRITE (952)

#define XP_ORB_ATTRACT_RANGE (48)

struct xp_orb_Data {
    u16 time;

    // initial jump, in 1/16 pixels per tick
    i8 xv;
    i8 yv;

    u8 unused[4];
};

static_assert(
    sizeof(struct xp_orb_Data) == 8,
    "struct xp_orb_Data: wrong size"
);

THUMB
void entity_add_xp_orb(struct Level *level, u16 x, u16 y) {
    u8 entity_id = level_new_entity(level, XP_ORB_ENTITY);
    if(entity_id >= ENTITY_LIMIT)
        return;

    struct entity_Data *data = &level->entities[entity_id];
    struct xp_orb_Data *orb_data = (struct xp_orb_Data *) &data->data;

    data->x = x - 4 + random(9);
    data->y = y - 4 + random(9);

    orb_data->time = 20 * 60 + random(60);
    orb_data->xv = random(33) - 16;
    orb_data->yv = random(33) - 16;

    level_add_entity(level, entity_id);
}

ETICK_ROM(xp_orb_tick) {
    struct xp_orb_Data *orb_data = (struct xp_orb_Data *) &data->data;

    orb_data->time--;
    if(orb_data->time == 0) {
        data->should_remove = true;
        return;
    }

    // Scatter a little when spawned. Not 'entity_move': that would call
    // 'touch_player', which an orb doesn't have.
    if(orb_data->xv != 0 || orb_data->yv != 0) {
        const u16 x = data->x + orb_data->xv / 8;
        const u16 y = data->y + orb_data->yv / 8;
        if(!LEVEL_GET_TILE_S(level, x >> 4, y >> 4)->is_solid) {
            data->x = x;
            data->y = y;
        }
        orb_data->xv -= (orb_data->xv > 0) - (orb_data->xv < 0);
        orb_data->yv -= (orb_data->yv > 0) - (orb_data->yv < 0);
    }

    struct entity_Data *player = &level->entities[0];
    if(player->type >= ENTITY_TYPES)
        return;

    const i32 xd = player->x - data->x;
    const i32 yd = player->y - data->y;
    const u32 dist = xd * xd + yd * yd;

    if(dist < 6 * 6) {
        if(player_xp < 65535)
            player_xp++;
        score++;

        SOUND_PLAY(sound_pickup);
        data->should_remove = true;
    } else if(dist < XP_ORB_ATTRACT_RANGE * XP_ORB_ATTRACT_RANGE) {
        data->x += (xd > 0) - (xd < 0);
        data->y += (yd > 0) - (yd < 0);
    }
}

EDRAW_ROM(xp_orb_draw) {
    struct xp_orb_Data *orb_data = (struct xp_orb_Data *) &data->data;

    // blink before disappearing
    if(orb_data->time < 2 * 60 && ((orb_data->time / 6) & 1) == 0)
        return 0;

    sprite_config(used_sprites, &(struct Sprite) {
        .x = data->x - 8 - level_x_offset,
        .y = data->y - 8 - level_y_offset,

        .priority = 2,

        .size = SPRITE_SIZE_16x16,
        .flip = 0,

        .tile = XP_ORB_SPRITE,
        .palette = 11
    });
    return 1;
}

const struct Entity xp_orb_entity = {
    .tick = xp_orb_tick,
    .draw = xp_orb_draw,

    .xr = 2,
    .yr = 2
};
