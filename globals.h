#ifndef _GLOBALS
#define _GLOBALS

#include <stdint.h>
#include <stdbool.h>
#include "bcd.h"
#include "typedefs.h"

#define STATE_TITLE				0
#define STATE_LEVEL				1

#define PLAYER_TILE_INDEX 0
#define PLAYER_ANIM_SPEED 14

#define PLAYER_BULLET_TILE_INDEX 4
#define EXPLOSION_TILE_INDEX 6
#define ENEMY_MULTI_TILE_INDEX 8

#define SPRITE_WIDTH 8
#define SPRITE_HEIGHT 16
#define HALF_SPRITE_HEIGHT SPRITE_HEIGHT / 2

// Because we are running in 8x16 sprite mode, all tile indexes should be multiplied by 2.
#define TILE_INDEX_MULTIPLIER 2

#define ENEMY_ARRAY_LENGTH 24
#define ENEMY_MOVEMENT_DELAY 8

extern uint8_t game_state;

extern BCD score;

extern player_t player;
extern bullet_t player_bullet;
extern enemy_t enemies[ENEMY_ARRAY_LENGTH];
extern explosion_t explosion;

extern uint8_t enemy_movement_timer;

// Used to determine movement for next turn.
extern uint8_t cur_leftmost_enemy_x;
extern uint8_t cur_rightmost_enemy_x;

extern uint8_t prev_rightmost_enemy_x;
extern uint8_t prev_leftmost_enemy_x;

// Movement for this turn.
extern int8_t movement_x;
extern int8_t movement_y;

extern bool enemies_move_left;

#endif