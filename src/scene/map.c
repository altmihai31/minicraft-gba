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
#include "scene.h"

#include "level.h"
#include "tile.h"
#include "entity.h"
#include "screen.h"

// The map item: the current level drawn at one pixel per tile. The
// 112x112 image is 14x14 BG tiles, generated into the free part of
// charblock 1 (tiles 256-451) and drawn with BG palette 11, which is
// otherwise only used for the active item icon.

#define MAP_TILE0   (256)
#define MAP_TILES_W (LEVEL_W / 8)
#define MAP_TILES_H (LEVEL_H / 8)

#define MAP_PALETTE (11)

#define MAP_X (8)
#define MAP_Y (3)

#define COLOR_ROCK      (1)
#define COLOR_GRASS     (2)
#define COLOR_LIQUID    (3)
#define COLOR_SAND      (4)
#define COLOR_TREE      (5)
#define COLOR_DIRT      (6)
#define COLOR_HARD      (7)
#define COLOR_FARM      (8)
#define COLOR_STAIRS    (9)
#define COLOR_CLOUD     (10)
#define COLOR_VOID      (11)
#define COLOR_PLAYER    (12)
#define COLOR_BUILDING  (13)
#define COLOR_ORE       (14)

static const u16 map_colors[16] = {
    [COLOR_ROCK]     = 0x294a,
    [COLOR_GRASS]    = 0x0a64,
    [COLOR_LIQUID]   = 0x5940,
    [COLOR_SAND]     = 0x2f7b,
    [COLOR_TREE]     = 0x0d82,
    [COLOR_DIRT]     = 0x152e,
    [COLOR_HARD]     = 0x2108,
    [COLOR_FARM]     = 0x1a94,
    [COLOR_STAIRS]   = 0x7fff,
    [COLOR_CLOUD]    = 0x6f7b,
    [COLOR_VOID]     = 0x0421,
    [COLOR_PLAYER]   = 0x001f,
    [COLOR_BUILDING] = 0x121c,
    [COLOR_ORE]      = 0x7314
};

#define LAVA_COLOR (0x011f)

static inline u32 tile_color(u32 tile) {
    switch(tile) {
        case GRASS_TILE:
        case FLOWER_TILE:
        case TREE_SAPLING_TILE:
        case PUMPKIN_TILE:
        case MELON_TILE:
        case SUGAR_CANE_TILE:
        case TORCH_GRASS_TILE:
            return COLOR_GRASS;

        case LIQUID_TILE:
        case HOLE_TILE:
        case ICE_TILE:
            return COLOR_LIQUID;

        case SAND_TILE:
        case CACTUS_TILE:
        case CACTUS_SAPLING_TILE:
        case TORCH_SAND_TILE:
            return COLOR_SAND;

        case TREE_TILE:
            return COLOR_TREE;

        case DIRT_TILE:
        case MUSHROOM_TILE:
        case TORCH_TILE:
            return COLOR_DIRT;

        case FARMLAND_TILE:
        case WHEAT_TILE:
            return COLOR_FARM;

        case STAIRS_DOWN_TILE:
        case STAIRS_UP_TILE:
            return COLOR_STAIRS;

        case CLOUD_TILE:
        case CLOUD_CACTUS_TILE:
            return COLOR_CLOUD;

        case INFINITE_FALL_TILE:
            return COLOR_VOID;

        case HARD_ROCK_TILE:
        case OBSIDIAN_TILE:
            return COLOR_HARD;

        case IRON_ORE_TILE:
        case GOLD_ORE_TILE:
        case GEM_ORE_TILE:
        case COAL_ORE_TILE:
        case SPAWNER_TILE:
            return COLOR_ORE;

        case PLANKS_TILE:
        case STONE_BRICKS_TILE:
        case CLAY_BRICKS_TILE:
        case DOOR_CLOSED_TILE:
        case DOOR_OPEN_TILE:
            return COLOR_BUILDING;
    }
    return COLOR_ROCK;
}

SBSS_SECTION
static bool should_render_game;

THUMB
static void map_init(u8 flags) {
    struct Level *level = &levels[current_level];
    struct entity_Data *player = &level->entities[0];

    const u32 pxt = player->x >> 4;
    const u32 pyt = player->y >> 4;

    vu32 *tiles = (vu32 *) display_charblock(1) + MAP_TILE0 * 8;

    for(u32 ty = 0; ty < MAP_TILES_H; ty++) {
        for(u32 tx = 0; tx < MAP_TILES_W; tx++) {
            vu32 *tile = tiles + (tx + ty * MAP_TILES_W) * 8;

            for(u32 row = 0; row < 8; row++) {
                const u32 yt = ty * 8 + row;

                u32 pixels = 0;
                for(u32 col = 0; col < 8; col++) {
                    const u32 xt = tx * 8 + col;

                    u32 color = tile_color(LEVEL_GET_TILE(level, xt, yt));

                    // 2x2 marker on the player
                    if((xt == pxt || xt == pxt + 1) &&
                       (yt == pyt || yt == pyt + 1))
                        color = COLOR_PLAYER;

                    pixels |= color << (col * 4);
                }
                tile[row] = pixels;
            }
        }
    }

    should_render_game = true;
}

THUMB
static void map_tick(void) {
    gametime++;

    if(input_press(KEY_A) || input_press(KEY_B) || input_press(KEY_START))
        set_scene(&scene_game, 1);
}

THUMB
static void map_draw(void) {
    if(should_render_game) {
        scene_game.draw();
        should_render_game = false;

        // after the game: its status bar reloads palette 11
        for(u32 i = 1; i < 16; i++)
            screen_set_bg_palette_color(MAP_PALETTE, i, map_colors[i]);

        // the deepest level's liquid is lava
        if(current_level == 0)
            screen_set_bg_palette_color(MAP_PALETTE, COLOR_LIQUID, LAVA_COLOR);
    }

    screen_draw_frame("MAP", MAP_X - 1, MAP_Y - 1,
                      MAP_TILES_W + 2, MAP_TILES_H + 2);

    for(u32 y = 0; y < MAP_TILES_H; y++) {
        for(u32 x = 0; x < MAP_TILES_W; x++) {
            const u16 tile = MAP_TILE0 + x + y * MAP_TILES_W;
            BG3_TILEMAP[(MAP_X + x) + (MAP_Y + y) * 32] =
                tile | MAP_PALETTE << 12;
        }
    }

    // blink the player marker
    const u16 color = ((tick_count >> 4) & 1) ? 0x7fff : 0x001f;
    screen_set_bg_palette_color(MAP_PALETTE, COLOR_PLAYER, color);
}

const struct Scene scene_map = {
    .init = map_init,

    .tick = map_tick,
    .draw = map_draw
};
