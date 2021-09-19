#ifndef _TYPEDEFS
#define _TYPEDEFS

#include <stdint.h>
#include <stdbool.h>
#include "bcd.h"

// Player
typedef struct Player {
  uint8_t sprite_count;
  uint8_t sprite_index;
  uint8_t location[2];
  uint8_t time_since_animation_start;
  uint8_t time_since_last_moved;
  uint8_t move_speed_delay;
  bool can_move;
  bool can_shoot;
} player_t;

// Player bullet
typedef struct Bullet {
  uint8_t sprite_count;
  uint8_t sprite_index;
  uint8_t location[2];
  uint8_t speed;
  uint8_t sprite_left_offset;
  uint8_t sprite_right_offset;
  uint8_t sprite_top_offset;
  uint8_t sprite_bottom_offset;
} bullet_t;

typedef struct Enemy {
  uint8_t sprite_index;
  uint8_t location[2];
  uint8_t sprite_left_offset;
  uint8_t sprite_right_offset;
  uint8_t sprite_top_offset;
  uint8_t sprite_bottom_offset;
  bool top_enemy;
  bool bottom_enemy;
  bool destroyed;
  BCD value;
  uint8_t bullet_cooldown;
  uint8_t bullet_timer;
} enemy_t;

typedef struct Explosion {
  uint8_t sprite_index;
  uint8_t location[2];
  uint8_t time_since_animation_start;
  uint8_t is_on_screen;
} explosion_t;

#endif