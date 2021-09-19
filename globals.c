#include <stdint.h>
#include <stdbool.h>
#include "globals.h"
#include "bcd.h"
#include "typedefs.h"

uint8_t curr_input;
uint8_t prev_input;

uint8_t game_state;

BCD score;
BCD high_score;

uint8_t seed;

player_t player;
bullet_t player_bullet;
enemy_t enemies[ENEMY_ARRAY_LENGTH];
explosion_t explosion;

uint8_t enemies_remaining;
uint8_t enemy_movement_timer;
uint8_t enemy_stage;
uint8_t enemies_left[24];
uint8_t enemies_right[24];
uint8_t next_wave_timer;
bullet_t enemy_bullets[5];
uint8_t enemy_bullets_count;
uint8_t enemy_bullets_cooldown;
uint8_t enemy_bullets_timer;
uint8_t enemy_which_will_shoot;

// Used to determine movement for next turn.
uint8_t cur_leftmost_enemy_x;
uint8_t cur_rightmost_enemy_x;

uint8_t prev_rightmost_enemy_x;
uint8_t prev_leftmost_enemy_x;

uint8_t lowest_enemy_y;

// Movement for this turn.
int8_t movement_x;
int8_t movement_y;

bool enemies_move_left;
bool enemies_move_down;
uint8_t enemies_move_delay;
uint8_t enemies_move_delay_decrease;