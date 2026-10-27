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
#include "generator.h"

#include "level.h"
#include "tile.h"
#include "entity.h"
#include "survival.h"

static inline i8 get(i8 *values, u32 x, u32 y) {
    if(x >= LEVEL_W || y >= LEVEL_H)
        return 0;
    return values[x + y * LEVEL_W];
}

static inline void set(i8 *values, u32 x, u32 y, i8 val) {
    if(x < LEVEL_W && y < LEVEL_H)
        values[x + y * LEVEL_W] = val;
}

static inline i32 variation(u32 step_size) {
    return (random(step_size * 4) - step_size * 2);
}

IWRAM_SECTION
static NO_INLINE i8 *noise(i8 *values, u32 feature_size) {
    for(u32 y = 0; y < LEVEL_H; y += feature_size)
        for(u32 x = 0; x < LEVEL_W; x += feature_size)
            set(values, x, y, random(256) - 128);

    for(u32 step_size = feature_size; step_size > 1; step_size /= 2) {
        u32 half_step = step_size / 2;

        for(u32 y = 0; y < LEVEL_H; y += step_size) {
            for(u32 x = 0; x < LEVEL_W; x += step_size) {
                //     |     |
                //        f
                //     |     |
                // - - a -h- b
                //     |     |
                //  g  i  e  |
                //     |     |
                // - - c --- d
                i8 a = get(values, x, y);
                i8 b = get(values, x + step_size, y);
                i8 c = get(values, x, y + step_size);
                i8 d = get(values, x + step_size, y + step_size);

                i8 e = (a + b + c + d) / 4 + variation(step_size);

                i8 f = get(values, x + half_step, y - half_step);
                i8 g = get(values, x - half_step, y + half_step);

                i8 h = (a + b + e + f) / 4 + variation(step_size) / 2;
                i8 i = (a + c + e + g) / 4 + variation(step_size) / 2;

                set(values, x + half_step, y + half_step, e);
                set(values, x + half_step, y, h);
                set(values, x, y + half_step, i);
            }
        }
    }
    return values;
}

#define abs math_abs

static inline bool generate_underground(u32 lvl) {
    struct Level *level = &levels[lvl];

    u32 rock_count   = 0;
    u32 dirt_count   = 0;
    u32 ore_count    = 0;
    u32 stairs_count = 0;

    i8 *noise1 = noise((i8 *) &levels[0].data, 32);
    i8 *noise2 = noise((i8 *) &levels[1].data, 32);

    i8 *mnoise1 = noise((i8 *) &levels[2].data, 16);
    i8 *mnoise2 = noise((i8 *) &levels[3].data, 16);
    i8 *mnoise3 = noise((i8 *) &levels[4].data, 16);

    i8 *nnoise1 = noise((i8 *) level_solid_entities + 1 * LEVEL_SIZE, 16);
    i8 *nnoise2 = noise((i8 *) level_solid_entities + 2 * LEVEL_SIZE, 16);
    i8 *nnoise3 = noise((i8 *) level_solid_entities + 3 * LEVEL_SIZE, 16);

    i8 *wnoise3 = noise((i8 *) level_solid_entities + 4 * LEVEL_SIZE, 16);

    for(u32 y = 0; y < LEVEL_H; y++) {
        for(u32 x = 0; x < LEVEL_W; x++) {
            u32 i = x + y * LEVEL_W;

            i32 val = abs(noise1[i] - noise2[i]) * 3 - 256;
            i32 mval = abs(abs(mnoise1[i] - mnoise2[i]) - mnoise3[i]) * 3 - 256;
            i32 nval = abs(abs(nnoise1[i] - nnoise2[i]) - nnoise3[i]) * 3 - 256;
            i32 wval = abs(nval - wnoise3[i]) * 3 - 256;

            // distance from center
            u32 xd = abs(x - LEVEL_W / 2) * 256 / LEVEL_W;
            u32 yd = abs(y - LEVEL_H / 2) * 256 / LEVEL_H;

            u32 dist = (xd >= yd) * xd + (xd < yd) * yd;
            dist = dist * dist * dist * dist / (128 * 128 * 128);
            dist = dist * dist * dist * dist / (128 * 128 * 128);
            val += 128 - dist * 20;

            if(val > -256 && wval < 384 * (lvl != 2) - 256) {
                level->tiles[i] = LIQUID_TILE;
            } else if(val > -256 && (mval < -218 || nval < -179)) {
                level->tiles[i] = DIRT_TILE;
                dirt_count++;
            } else {
                level->tiles[i] = ROCK_TILE;
                rock_count++;
            }
        }
    }

    // add ores
    for(u32 i = 0; i < LEVEL_SIZE / 400; i++) {
        // center of the ores
        u32 xc = random(LEVEL_W);
        u32 yc = random(LEVEL_H);

        for(u32 j = 0; j < 30; j++) {
            i32 xt = xc + random(9) - 4;
            i32 yt = yc + random(9) - 4;

            if(xt >= 2 && xt < LEVEL_W - 2 && yt >= 2 && yt < LEVEL_H - 2) {
                if(level->tiles[xt + yt * LEVEL_W] == ROCK_TILE) {
                    level->tiles[xt + yt * LEVEL_W] = (GEM_ORE_TILE - lvl);

                    ore_count++;
                    rock_count--;
                }
            }
        }
    }

    // add stairs down
    if(lvl != 0) {
        for(u32 i = 0; i < LEVEL_SIZE / 100; i++) {
            u32 xt = 10 + random(LEVEL_W - 20);
            u32 yt = 10 + random(LEVEL_H - 20);

            // check if there is rock all around
            for(u32 y = yt - 1; y <= yt + 1; y++)
                for(u32 x = xt - 1; x <= xt + 1; x++)
                    if(level->tiles[x + y * LEVEL_W] != ROCK_TILE)
                        goto continue_stairs_down;

            level->tiles[xt + yt * LEVEL_W] = STAIRS_DOWN_TILE;

            stairs_count++;
            rock_count--;

            if(stairs_count >= 4)
                break;

            continue_stairs_down:;
        }
    }

    return rock_count >= 100 && dirt_count >= 100 && ore_count >= 20 &&
           (stairs_count >= 2 || lvl == 0);
}

static inline bool generate_top(void) {
    struct Level *level = &levels[3];

    u32 rock_count   = 0;
    u32 grass_count  = 0;
    u32 sand_count   = 0;
    u32 tree_count   = 0;
    u32 stairs_count = 0;

    i8 *noise1 = noise((i8 *) &levels[0].data, 32);
    i8 *noise2 = noise((i8 *) &levels[1].data, 32);

    i8 *mnoise1 = noise((i8 *) &levels[2].data, 16);
    i8 *mnoise2 = noise((i8 *) &levels[3].data, 16);
    i8 *mnoise3 = noise((i8 *) &levels[4].data, 16);

    for(u32 y = 0; y < LEVEL_H; y++) {
        for(u32 x = 0; x < LEVEL_W; x++) {
            u32 i = x + y * LEVEL_W;

            i32 val = abs(noise1[i] - noise2[i]) * 3 - 256;
            i32 mval = abs(abs(mnoise1[i] - mnoise2[i]) - mnoise3[i]) * 3 - 256;

            // distance from center
            u32 xd = abs(x - LEVEL_W / 2) * 256 / LEVEL_W;
            u32 yd = abs(y - LEVEL_H / 2) * 256 / LEVEL_H;

            u32 dist = (xd >= yd) * xd + (xd < yd) * yd;
            dist = dist * dist * dist * dist / (128 * 128 * 128);
            dist = dist * dist * dist * dist / (128 * 128 * 128);
            val += 128 - dist * 20;

            if(val < -64) {
                level->tiles[i] = LIQUID_TILE;
            } else if(val > 64 && mval < -192) {
                level->tiles[i] = ROCK_TILE;
                rock_count++;
            } else {
                level->tiles[i] = GRASS_TILE;
                grass_count++;
            }
        }
    }

    // add deserts
    for(u32 i = 0; i < LEVEL_SIZE / 2800; i++) {
        // center of the desert
        u32 xc = random(LEVEL_W);
        u32 yc = random(LEVEL_H);

        for(u32 j = 0; j < 10; j++) {
            i32 xj = xc + random(21) - 10;
            i32 yj = yc + random(21) - 10;

            for(u32 k = 0; k < 85; k++) {
                i32 xk = xj + random(9) - 4;
                i32 yk = yj + random(9) - 4;

                for(i32 yt = yk - 1; yt <= yk + 1; yt++) {
                    if(yt < 0 || yt >= LEVEL_H)
                        continue;

                    for(i32 xt = xk - 1; xt <= xk + 1; xt++) {
                        if(xt < 0 || xt >= LEVEL_W)
                            continue;

                        if(level->tiles[xt + yt * LEVEL_W] == GRASS_TILE) {
                            level->tiles[xt + yt * LEVEL_W] = SAND_TILE;

                            sand_count++;
                            grass_count--;
                        }
                    }
                }
            }
        }
    }

    // add forests
    for(u32 i = 0; i < LEVEL_SIZE / 400; i++) {
        // center of the forest
        u32 xc = random(LEVEL_W);
        u32 yc = random(LEVEL_H);

        for(u32 j = 0; j < 175; j++) {
            i32 xt = xc + random(29) - 14;
            i32 yt = yc + random(29) - 14;

            if(xt >= 0 && xt < LEVEL_W && yt >= 0 && yt < LEVEL_H) {
                if(level->tiles[xt + yt * LEVEL_W] == GRASS_TILE) {
                    level->tiles[xt + yt * LEVEL_W] = TREE_TILE;

                    tree_count++;
                    grass_count--;
                }
            }
        }
    }

    // add flowers
    for(u32 i = 0; i < LEVEL_SIZE / 400; i++) {
        // center of the flowers
        u32 xc = random(LEVEL_W);
        u32 yc = random(LEVEL_H);

        for(u32 j = 0; j < 30; j++) {
            i32 xt = xc + random(9) - 4;
            i32 yt = yc + random(9) - 4;

            if(xt >= 0 && xt < LEVEL_W && yt >= 0 && yt < LEVEL_H) {
                if(level->tiles[xt + yt * LEVEL_W] == GRASS_TILE) {
                    level->tiles[xt + yt * LEVEL_W] = FLOWER_TILE;

                    grass_count--;
                }
            }
        }
    }

    // add cactus
    for(u32 i = 0; i < LEVEL_SIZE / 100; i++) {
        u32 xt = random(LEVEL_W);
        u32 yt = random(LEVEL_H);

        if(level->tiles[xt + yt * LEVEL_W] == SAND_TILE) {
            level->tiles[xt + yt * LEVEL_W] = CACTUS_TILE;

            sand_count--;
        }
    }

    // add stairs down
    for(u32 i = 0; i < LEVEL_SIZE / 100; i++) {
        u32 xt = 1 + random(LEVEL_W - 2);
        u32 yt = 1 + random(LEVEL_H - 2);

        // check if there is rock all around
        for(u32 y = yt - 1; y <= yt + 1; y++)
            for(u32 x = xt - 1; x <= xt + 1; x++)
                if(level->tiles[x + y * LEVEL_W] != ROCK_TILE)
                    goto continue_stairs_down;

        level->tiles[xt + yt * LEVEL_W] = STAIRS_DOWN_TILE;

        stairs_count++;
        rock_count--;

        if(stairs_count >= 4)
            break;

        continue_stairs_down:;
    }

    return rock_count >= 100 && grass_count >= 100 &&
           sand_count >= 100 && tree_count  >= 100 &&
           stairs_count >= 2;
}

static inline bool generate_sky(void) {
    struct Level *level = &levels[4];

    u32 cloud_count  = 0;
    u32 stairs_count = 0;

    i8 *noise1 = noise((i8 *) &levels[0].data, 8);
    i8 *noise2 = noise((i8 *) &levels[1].data, 8);

    for(u32 y = 0; y < LEVEL_H; y++) {
        for(u32 x = 0; x < LEVEL_W; x++) {
            u32 i = x + y * LEVEL_W;

            i32 val = abs(noise1[i] - noise2[i]) * 3 - 256;

            // distance from center
            u32 xd = abs(x - LEVEL_W / 2) * 256 / LEVEL_W;
            u32 yd = abs(y - LEVEL_H / 2) * 256 / LEVEL_H;

            u32 dist = (xd >= yd) * xd + (xd < yd) * yd;
            dist = dist * dist * dist * dist / (128 * 128 * 128);
            dist = dist * dist * dist * dist / (128 * 128 * 128);
            val = -val - 282;
            val += 128 - dist * 20;

            if(val < -32) {
                level->tiles[i] = INFINITE_FALL_TILE;
            } else {
                level->tiles[i] = CLOUD_TILE;
                cloud_count++;
            }
        }
    }

    // add cloud cactus
    for(u32 i = 0; i < LEVEL_SIZE / 50; i++) {
        u32 xt = 1 + random(LEVEL_W - 2);
        u32 yt = 1 + random(LEVEL_H - 2);

        // check if there is cloud all around
        for(u32 y = yt - 1; y <= yt + 1; y++)
            for(u32 x = xt - 1; x <= xt + 1; x++)
                if(level->tiles[x + y * LEVEL_W] != CLOUD_TILE)
                    goto continue_cloud_cactus;

        level->tiles[xt + yt * LEVEL_W] = CLOUD_CACTUS_TILE;
        cloud_count--;

        continue_cloud_cactus:;
    }

    // add stairs down
    for(u32 i = 0; i < LEVEL_SIZE / 100; i++) {
        u32 xt = 1 + random(LEVEL_W - 2);
        u32 yt = 1 + random(LEVEL_H - 2);

        // check if there is cloud all around
        for(u32 y = yt - 1; y <= yt + 1; y++)
            for(u32 x = xt - 1; x <= xt + 1; x++)
                if(level->tiles[x + y * LEVEL_W] != CLOUD_TILE)
                    goto continue_stairs_down;

        level->tiles[xt + yt * LEVEL_W] = STAIRS_DOWN_TILE;

        stairs_count++;
        cloud_count--;

        if(stairs_count >= 4)
            break;

        continue_stairs_down:;
    }

    return cloud_count >= 1750 && stairs_count >= 2;
}

static inline void generate_stairs_up(void) {
    for(u32 l = 0; l < 4; l++) {
        for(u32 i = 0; i < LEVEL_SIZE; i++) {
            if(levels[l + 1].tiles[i] == STAIRS_DOWN_TILE) {
                u8 border_tile = l == 3 ? HARD_ROCK_TILE : DIRT_TILE;

                // clear area around
                const u32 xt = i % LEVEL_W;
                const u32 yt = i / LEVEL_W;
                for(u32 y = yt - 1; y <= yt + 1; y++)
                    for(u32 x = xt - 1; x <= xt + 1; x++)
                        levels[l].tiles[x + y * LEVEL_W] = border_tile;

                levels[l].tiles[i] = STAIRS_UP_TILE;
            }
        }
    }
}

static inline void generate_data(void) {
    for(u32 l = 0; l < 5; l++)
        for(u32 i = 0; i < LEVEL_SIZE; i++)
            levels[l].data[i] = 0;

    // flower data
    struct Level *top_level = &levels[3];
    for(u32 i = 0; i < LEVEL_SIZE; i++) {
        if(top_level->tiles[i] == FLOWER_TILE)
            top_level->data[i] = random(2);
    }
}

static inline void generate_entities(void) {
    // clear all entities
    for(u32 l = 0; l < 5; l++)
        for(u32 i = 1; i < ENTITY_LIMIT; i++)
            levels[l].entities[i].type = -1;

    // spawn player
    while(true) {
        u32 i = random(LEVEL_SIZE);

        if(levels[3].tiles[i] == GRASS_TILE) {
            entity_add_player(
                &levels[3],
                i % LEVEL_W, i / LEVEL_W,
                true
            );
            break;
        }
    }

    // spawn air wizard
    entity_add_air_wizard(&levels[4]);
}

/* ================================================================== */
/*                            Mod features                            */
/* ================================================================== */

#define TILE_AT(level, x, y) ((level)->tiles[(x) + (y) * LEVEL_W])

static inline bool next_to_liquid(struct Level *level, u32 x, u32 y) {
    return TILE_AT(level, x - 1, y) == LIQUID_TILE ||
           TILE_AT(level, x + 1, y) == LIQUID_TILE ||
           TILE_AT(level, x, y - 1) == LIQUID_TILE ||
           TILE_AT(level, x, y + 1) == LIQUID_TILE;
}

// replace 'from' with 'to' in clusters around random points
THUMB
static NO_INLINE void add_clusters(struct Level *level, u32 clusters,
                                   u32 tries, u32 spread,
                                   u8 from, u8 to) {
    for(u32 i = 0; i < clusters; i++) {
        const u32 xc = random(LEVEL_W);
        const u32 yc = random(LEVEL_H);

        for(u32 j = 0; j < tries; j++) {
            const i32 x = xc + random(spread * 2 + 1) - spread;
            const i32 y = yc + random(spread * 2 + 1) - spread;

            if(x >= 1 && x < LEVEL_W - 1 && y >= 1 && y < LEVEL_H - 1 &&
               TILE_AT(level, x, y) == from)
                TILE_AT(level, x, y) = to;
        }
    }
}

// A dungeon: a 7x7 room of stone bricks with a spawner in the middle
THUMB
static NO_INLINE void add_dungeons(struct Level *level, u32 count) {
    for(u32 attempt = 0; attempt < 200 && count > 0; attempt++) {
        const u32 xc = 6 + random(LEVEL_W - 12);
        const u32 yc = 6 + random(LEVEL_H - 12);

        // only inside solid rock, so it doesn't cut through anything
        for(u32 y = yc - 3; y <= yc + 3; y++)
            for(u32 x = xc - 3; x <= xc + 3; x++)
                if(TILE_AT(level, x, y) != ROCK_TILE)
                    goto next_attempt;

        for(u32 y = yc - 3; y <= yc + 3; y++) {
            for(u32 x = xc - 3; x <= xc + 3; x++) {
                const bool wall = (x == xc - 3 || x == xc + 3 ||
                                   y == yc - 3 || y == yc + 3);
                TILE_AT(level, x, y) = wall ? STONE_BRICKS_TILE : DIRT_TILE;
            }
        }
        TILE_AT(level, xc, yc) = SPAWNER_TILE;

        // one opening on a random side
        switch(random(4)) {
            case 0: TILE_AT(level, xc, yc - 3) = DIRT_TILE; break;
            case 1: TILE_AT(level, xc - 3, yc) = DIRT_TILE; break;
            case 2: TILE_AT(level, xc, yc + 3) = DIRT_TILE; break;
            case 3: TILE_AT(level, xc + 3, yc) = DIRT_TILE; break;
        }
        count--;

        next_attempt:;
    }
}

THUMB
static NO_INLINE void generate_mod_tiles(void) {
    struct Level *top = &levels[3];

    // coal ore: surface rock and the first underground level
    add_clusters(top,        LEVEL_SIZE / 600, 20, 3, ROCK_TILE, COAL_ORE_TILE);
    add_clusters(&levels[2], LEVEL_SIZE / 400, 20, 3, ROCK_TILE, COAL_ORE_TILE);

    // pumpkin and melon patches
    add_clusters(top, 8, 8, 2, GRASS_TILE, PUMPKIN_TILE);
    add_clusters(top, 8, 8, 2, GRASS_TILE, MELON_TILE);

    // mushrooms on underground dirt
    add_clusters(&levels[2], 30, 6, 3, DIRT_TILE, MUSHROOM_TILE);
    add_clusters(&levels[1], 30, 6, 3, DIRT_TILE, MUSHROOM_TILE);

    // sugar cane along the water
    for(u32 i = 0; i < LEVEL_SIZE / 8; i++) {
        const u32 x = 1 + random(LEVEL_W - 2);
        const u32 y = 1 + random(LEVEL_H - 2);

        const u8 tile = TILE_AT(top, x, y);
        if((tile == GRASS_TILE || tile == SAND_TILE) &&
           next_to_liquid(top, x, y) && random(3) == 0)
            TILE_AT(top, x, y) = SUGAR_CANE_TILE;
    }

    // frozen ponds
    for(u32 i = 0; i < 4; i++) {
        for(u32 attempt = 0; attempt < 100; attempt++) {
            const u32 x = random(LEVEL_W);
            const u32 y = random(LEVEL_H);

            if(TILE_AT(top, x, y) == LIQUID_TILE) {
                for(u32 j = 0; j < 60; j++) {
                    const i32 xi = x + random(9) - 4;
                    const i32 yi = y + random(9) - 4;

                    if(xi >= 0 && xi < LEVEL_W && yi >= 0 && yi < LEVEL_H &&
                       TILE_AT(top, xi, yi) == LIQUID_TILE)
                        TILE_AT(top, xi, yi) = ICE_TILE;
                }
                break;
            }
        }
    }

    // dungeons with spawners
    add_dungeons(&levels[1], 3);
    add_dungeons(&levels[0], 3);
}

THUMB
static NO_INLINE void generate_mod_entities(void) {
    spawn_level = NO_SPAWN_POINT;

    // the magma golem guards the deepest level
    for(u32 attempt = 0; attempt < 10000; attempt++) {
        const u32 i = random(LEVEL_SIZE);
        if(levels[0].tiles[i] == DIRT_TILE) {
            entity_add_mod_mob(
                &levels[0], MAGMA_GOLEM_ENTITY,
                ((i % LEVEL_W) << 4) + 8, ((i / LEVEL_W) << 4) + 8
            );
            break;
        }
    }

    // some animals to start with
    for(u32 n = 0; n < 30; n++) {
        const u32 i = random(LEVEL_SIZE);
        if(levels[3].tiles[i] != GRASS_TILE)
            continue;

        entity_add_mod_mob(
            &levels[3], COW_ENTITY + random(4),
            ((i % LEVEL_W) << 4) + 8, ((i / LEVEL_W) << 4) + 8
        );
    }
}

void generate_levels(void) {
    while(!generate_underground(0));
    while(!generate_underground(1));
    while(!generate_underground(2));
    while(!generate_top());
    while(!generate_sky());

    generate_stairs_up();
    generate_mod_tiles();
    generate_data();

    generate_entities();
    generate_mod_entities();
}
