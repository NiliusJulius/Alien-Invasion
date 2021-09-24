#ifndef _GLOBALS
#define _GLOBALS

#include <stdint.h>
#include <stdbool.h>
#include "bcd.h"
#include "typedefs.h"

#define UPDATE_KEYS()	prev_input = curr_input; curr_input = joypad()
#define KEY_PRESSED(K)	(curr_input & (K))
#define KEY_DEBOUNCE(K) ((curr_input & (K)) && (prev_input & (K)))
#define KEY_TICKED(K)   ((curr_input & (K)) && !(prev_input & (K)))
#define KEY_RELEASED(K) ((prev_input & (K)) && !(curr_input & (K)))

#define STATE_INTRO 0
#define STATE_GAME 1
#define STATE_GAME_OVER 2

#define PLAYER_TILE_INDEX 0
#define PLAYER_ANIM_SPEED 14
#define PLAYER_START_HEIGHT 144
#define PLAYER_START_WIDTH 30

#define PLAYER_BULLET_TILE_INDEX 4
#define EXPLOSION_TILE_INDEX 6
#define ENEMY_MULTI_TILE_INDEX 8
#define ENEMY_SECOND_FRAME_TILE_INDEX 64 + ENEMY_MULTI_TILE_INDEX
#define ENEMY_BULLETS_TILE_INDEX 4

#define ENEMY_BULLETS_SPRITE_INDEX 10
#define ENEMY_BULLETS_ARRAY_LENGTH 5
#define ENEMY_BULLETS_COOLDOWN 80
#define ENEMY_BULLETS_DECREASE 10

#define SPRITE_WIDTH 8
#define SPRITE_HEIGHT 16
#define HALF_SPRITE_HEIGHT SPRITE_HEIGHT / 2
#define HALF_SCREEN_WIDTH 80

// Because we are running in 8x16 sprite mode, all tile indexes should be multiplied by 2.
#define TILE_INDEX_MULTIPLIER 2

#define ENEMY_TYPES 2
#define ENEMY_GROUPS_COUNT 3
#define ENEMY_ARRAY_LENGTH 21
#define ENEMIES_PER_ROW 7
#define ENEMY_MOVEMENT_DELAY 48
#define ENEMY_MOVEMENT_DELAY_DECREASE 2
#define ENEMY_MOVEMENT_DELAY_MIN 5

extern uint8_t curr_input;
extern uint8_t prev_input;

extern uint8_t game_state;

extern BCD score;
extern BCD high_score;

extern uint8_t seed;

extern player_t player;
extern bullet_t player_bullet;
extern enemy_t enemies[ENEMY_ARRAY_LENGTH];
extern explosion_t explosion;

extern uint8_t enemies_remaining;
extern uint8_t enemy_movement_timer;
extern uint8_t enemy_stage;
extern uint8_t enemies_left[24];
extern uint8_t enemies_right[24];
extern uint8_t next_wave_timer;
extern bullet_t enemy_bullets[5];
extern uint8_t enemy_bullets_count;
extern uint8_t enemy_bullets_cooldown;
extern uint8_t enemy_bullets_timer;
extern uint8_t lowest_enemies[7];
extern uint8_t enemy_which_will_shoot;

// Used to determine movement for next turn.
extern uint8_t cur_leftmost_enemy_x;
extern uint8_t cur_rightmost_enemy_x;

extern uint8_t prev_rightmost_enemy_x;
extern uint8_t prev_leftmost_enemy_x;

extern uint8_t lowest_enemy_y;

// Movement for this turn.
extern int8_t movement_x;
extern int8_t movement_y;

extern bool enemies_move_left;
extern bool enemies_move_down;
extern uint8_t enemies_move_delay;
extern uint8_t enemies_move_delay_decrease;

#endif