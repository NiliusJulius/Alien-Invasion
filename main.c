#include <gb/gb.h>
#include <gb/font.h>
#include <stdio.h>
#include "bcd.h"
#include "typedefs.c"
#include "enemy_groups.c"
#include "music.h"
#include "sound.h"
#include "sprites\player_sprites.h"
#include "sprites\bullet_sprites.h"
#include "sprites\explosion_sprites.h"
#include "sprites\multiple_enemy_sprites.h"
#include "maps\window_map.h"

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

BCD score = MAKE_BCD(0);

player_t player;
bullet_t player_bullet;
enemy_t enemies[ENEMY_ARRAY_LENGTH];
explosion_t explosion;

void performant_delay(uint8_t num_loops){
    uint8_t i;
    for(i = 0; i < num_loops; i++){
        wait_vbl_done();
    }     
}

void animate_player() {
  uint8_t current_frame;
  if (player.time_since_animation_start >= player.sprite_count * PLAYER_ANIM_SPEED) {
    // If we passed the last frame time, reset the timer.
    player.time_since_animation_start = 0;
    current_frame = 0;
  } else {
    // else find the frame we should be at.
    current_frame = player.time_since_animation_start / PLAYER_ANIM_SPEED;
  }

  set_sprite_tile(player.sprite_index, current_frame * TILE_INDEX_MULTIPLIER);
  player.time_since_animation_start = player.time_since_animation_start + 1;
}

void update_explosion() {
  if (explosion.is_on_screen) {
    if (explosion.time_since_animation_start >= 6) {
      explosion.location[0] = 0;
      explosion.location[1] = 0;
      move_sprite(explosion.sprite_index, explosion.location[0], explosion.location[1]);
      explosion.is_on_screen = FALSE;
    }
    explosion.time_since_animation_start = explosion.time_since_animation_start + 1;
  }
}

void update_player() {
  if (player.time_since_last_moved >= player.move_speed_delay) {
    player.can_move = TRUE;
    player.time_since_last_moved = 0;
  } else if (!player.can_move) {
    player.time_since_last_moved++;
  }
}

void update_player_bullet() {
  if (player_bullet.location[1] + SPRITE_HEIGHT > 16 && player_bullet.location[1] < 160) {
    player_bullet.location[1] = player_bullet.location[1] - 1 * player_bullet.speed;
    move_sprite(player_bullet.sprite_index, player_bullet.location[0], player_bullet.location[1]);
  } else {
    player.can_shoot = TRUE;
  }
}

void update_enemies() {
  for (uint8_t i=0; i<ENEMY_ARRAY_LENGTH; i++) {
    // Already destroyed enemies don't need updating.
    if (!enemies[i].destroyed) {
      // Check whether the enemy is being hit by the player's bullet.
      if (player_bullet.location[0] + player_bullet.sprite_left_offset < enemies[i].location[0] + enemies[i].sprite_right_offset + SPRITE_WIDTH &&
              player_bullet.location[0] - player_bullet.sprite_right_offset + SPRITE_WIDTH > enemies[i].location[0] - enemies[i].sprite_left_offset &&
              player_bullet.location[1] + player_bullet.sprite_top_offset < enemies[i].location[1] - enemies[i].sprite_bottom_offset + SPRITE_HEIGHT &&
              player_bullet.location[1] - player_bullet.sprite_bottom_offset + SPRITE_HEIGHT > enemies[i].location[1] + enemies[i].sprite_top_offset
      ) {
        // Set the explosion x location first, since we will move the destroyed enemies.
        explosion.location[0] = enemies[i].location[0];

        // Play explosion sound.
        set_sound(SOUND_EXPLOSION);

        // Top enemy hit.
        if ((player_bullet.location[1] + player_bullet.sprite_top_offset <= enemies[i].location[1] + HALF_SPRITE_HEIGHT)) {
          explosion.location[1] = enemies[i].location[1];
          // If the bottom enemy still exists, we update to only show that one.
          if (enemies[i].bottom_enemy) {
            enemies[i].top_enemy = FALSE;
            enemies[i].sprite_top_offset = 8;
            set_sprite_tile(enemies[i].sprite_index, ENEMY_MULTI_TILE_INDEX + 4);
          } else {
            // Enemy is totally destroyed.
            enemies[i].destroyed = TRUE;
            enemies[i].location[0] = 0;
            enemies[i].location[1] = 0;
            move_sprite(enemies[i].sprite_index, 0, 0);
          }
        } else {
          // Bottom enemy hit.
          explosion.location[1] = enemies[i].location[1] + HALF_SPRITE_HEIGHT;
          // If the top enemy still exists, we update to only show that one.
          if (enemies[i].top_enemy) {
            enemies[i].bottom_enemy = FALSE;
            enemies[i].sprite_bottom_offset = 1 + 8;
            set_sprite_tile(enemies[i].sprite_index, ENEMY_MULTI_TILE_INDEX + 2);
          } else {
            // Enemy is totally destroyed.
            enemies[i].destroyed = TRUE;
            enemies[i].location[0] = 0;
            enemies[i].location[1] = 0;
            move_sprite(enemies[i].sprite_index, 0, 0);
          }
        }

        // Update the explosion to show on the enemies location.
        explosion.time_since_animation_start = 0;
        move_sprite(explosion.sprite_index, explosion.location[0], explosion.location[1]);
        explosion.is_on_screen = TRUE;

        // Destroy the player's bullet.
        player_bullet.location[0] = 0;
        player_bullet.location[1] = 1;
        move_sprite(player_bullet.sprite_index, player_bullet.location[0], player_bullet.location[1]);

        // Update the score.
        bcd_add(&score, &enemies[i].value);
      }
    }
  }
}

void update_window() {
  unsigned char buffer[9];
  bcd2text(&score, 0x01, buffer);
  set_win_tiles(6, 0, 8, 1, buffer);
}

void createEnemies() {
  enemy_t *enemy = enemies;
  for (uint8_t i=0; i<ENEMY_ARRAY_LENGTH; i++) {
    enemy->sprite_index = 15 + i;
    enemy->location[0] = 24 + SPRITE_WIDTH * 2 * (i % 8);
    enemy->location[1] = 30 + SPRITE_HEIGHT * (i / 8);
    enemy->destroyed = FALSE;
    enemy->value = MAKE_BCD(100);
    move_sprite(enemy->sprite_index, enemy->location[0], enemy->location[1]);

    switch (enemy_group1[i])
    {
    case 1:
      enemy->sprite_top_offset = 0;
      enemy->sprite_bottom_offset = 1;
      enemy->sprite_left_offset = 0;
      enemy->sprite_right_offset = 0;
      enemy->top_enemy = TRUE;
      enemy->bottom_enemy = TRUE;
      set_sprite_tile(enemy->sprite_index, ENEMY_MULTI_TILE_INDEX);
      break;
    case 2:
      enemy->sprite_top_offset = 0;
      enemy->sprite_bottom_offset = 1 + HALF_SPRITE_HEIGHT;
      enemy->sprite_left_offset = 0;
      enemy->sprite_right_offset = 0;
      enemy->top_enemy = TRUE;
      enemy->bottom_enemy = FALSE;
      set_sprite_tile(enemy->sprite_index, ENEMY_MULTI_TILE_INDEX + 2);
      break;
    case 3:
      enemy->sprite_top_offset = HALF_SPRITE_HEIGHT;
      enemy->sprite_bottom_offset = 1;
      enemy->sprite_left_offset = 0;
      enemy->sprite_right_offset = 0;
      enemy->top_enemy = FALSE;
      enemy->bottom_enemy = TRUE;
      set_sprite_tile(enemy->sprite_index, ENEMY_MULTI_TILE_INDEX + 4);
      break;
    }

    enemy++;
  }
}

void main() {
  SPRITES_8x16;
  SHOW_SPRITES;
  SHOW_WIN;

  // Initialize sound and music.
  sound_cnt_ch1 = 0;
  sound_cnt_ch4 = 0;
  init_sound();
  set_music(MUSIC_INTRO);

  // Create font.
  font_t min_font;
  font_init();
  min_font = font_load(font_min);
  font_set(min_font);

  // Create the score text on screen.
  unsigned char scoreText[] = {0x1D, 0xD, 0x19, 0x1C, 0xF};
  set_win_tiles(0, 0, 5, 1, scoreText);

  // Set initial values of the player instance.
  player.sprite_count = 2;
  player.sprite_index = 0;
  player.location[0] = 30;
  player.location[1] = 140;
  player.time_since_animation_start = 0;
  player.time_since_last_moved = 0;
  player.move_speed_delay = 0;
  player.can_move = TRUE;
  player.can_shoot = TRUE;
  // Set player sprite data.
  set_sprite_data(PLAYER_TILE_INDEX, player.sprite_count * 2, player_sprites);
  move_sprite(player.sprite_index, player.location[0], player.location[1]);

  // Set initial values of the player bullet instance.
  player_bullet.sprite_count = 1;
  player_bullet.sprite_index = 1;
  player_bullet.location[0] = 0;
  player_bullet.location[1] = 0;
  player_bullet.speed = 3;
  player_bullet.sprite_left_offset = 3;
  player_bullet.sprite_right_offset = 3;
  player_bullet.sprite_top_offset = 0;
  player_bullet.sprite_bottom_offset = 8;
  // Set player bullet sprite data.
  set_sprite_data(PLAYER_BULLET_TILE_INDEX, player_bullet.sprite_count * TILE_INDEX_MULTIPLIER, bullet_sprites);
  set_sprite_tile(player_bullet.sprite_index, PLAYER_BULLET_TILE_INDEX);

  // Set initial values of enemy instances.
  createEnemies();
  // Set enemy sprite data.
  set_sprite_data(ENEMY_MULTI_TILE_INDEX, 3 * TILE_INDEX_MULTIPLIER, multiple_enemy_sprites);

  // Set initial values of the explosion instance.
  explosion.sprite_index = 39;
  explosion.location[0] = 0;
  explosion.location[1] = 0;
  explosion.is_on_screen = FALSE;
  // Set explostion sprite data.
  set_sprite_data(EXPLOSION_TILE_INDEX, 1 * TILE_INDEX_MULTIPLIER, explosion_sprites);
  set_sprite_tile(explosion.sprite_index, EXPLOSION_TILE_INDEX);

  // Game loop.
  while (1) {
    
    // Player controls
    if (player.can_move) {
      if (joypad()&J_LEFT) {
        if (player.location[0] - 1 >= 8) {
          player.location[0]--;
          move_sprite(player.sprite_index, player.location[0], player.location[1]);
          player.can_move = FALSE;
        }
      }
      if (joypad()&J_RIGHT) {
        if (player.location[0] + SPRITE_WIDTH + 1 <= 168) {
          player.location[0]++;
          move_sprite(player.sprite_index, player.location[0], player.location[1]);
          player.can_move = FALSE;
        }
      }
    }
    if (joypad()&J_A) {
      if (player.can_shoot) {
        player_bullet.location[0] = player.location[0];
        player_bullet.location[1] = player.location[1] - 1;
        move_sprite(player_bullet.sprite_index, player_bullet.location[0], player_bullet.location[1]);
        player.can_shoot = FALSE;
        set_sound(SOUND_PLAYER_SHOOT);
      }
    }

    // If music and/or sound are set, play them.
    play_music();
    play_sound();

    // Update everything for the player.
    update_player();
    animate_player();

    update_enemies();

    update_explosion();
    
    update_player_bullet();
    
    update_window();

    performant_delay(1);
  }
  
}