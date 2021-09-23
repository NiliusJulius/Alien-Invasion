#include <stdint.h>
#include <stdbool.h>
#include <gb\gb.h>
#include <rand.h>
#include "..\typedefs.h"
#include "..\globals.h"
#include "game_over.h"
#include "..\sound\sound.h"
#include "..\sound\music.h"
#include "..\levels\enemy_groups.h"

#include "..\sprites\player_sprites.h"
#include "..\sprites\bullet_sprites.h"
#include "..\sprites\explosion_sprites.h"
#include "..\sprites\multiple_enemy_sprites.h"
#include "..\sprites\multiple_enemy_sprites_sec.h"

void game_over() {
  init_game_over();
  game_state = STATE_GAME_OVER;
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
  set_sprite_tile(player.sprite_index + 1, current_frame * TILE_INDEX_MULTIPLIER);
  player.time_since_animation_start = player.time_since_animation_start + 1;
}

void move_player() {
  move_sprite(player.sprite_index, player.location[0], player.location[1]);
  move_sprite(player.sprite_index + 1, player.location[0] + SPRITE_WIDTH, player.location[1]);
}

void update_explosion() {
  if (explosion.is_on_screen) {
    if (explosion.time_since_animation_start >= 6) {
      explosion.location[0] = 0;
      explosion.location[1] = 0;
      move_sprite(explosion.sprite_index, explosion.location[0], explosion.location[1]);
      explosion.is_on_screen = false;
    }
    explosion.time_since_animation_start = explosion.time_since_animation_start + 1;
  }
}

void update_player() {
  if (player.time_since_last_moved >= player.move_speed_delay) {
    player.can_move = true;
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
    player.can_shoot = true;
  }
}

/* 
  #################
  START ENEMY LOGIC
  #################
*/

void createEnemies() {
  // Reset bullets
  enemy_bullets_count = 0;
  enemy_bullets_timer = 0;
  for (uint8_t j = 0; j < ENEMY_BULLETS_ARRAY_LENGTH; j++) {
    enemy_bullets[j].location[0] = 0;
    enemy_bullets[j].location[1] = 0; 
    move_sprite(enemy_bullets[j].sprite_index, 0, 0);
  }

  enemy_t *enemy = enemies;
  for (uint8_t i=0; i<ENEMY_ARRAY_LENGTH; i++) {
    enemy->sprite_index = 15 + i;
    enemy->location[0] = 36 + SPRITE_WIDTH * 2 * (i % ENEMIES_PER_ROW);
    enemy->location[1] = 30 + SPRITE_HEIGHT * (i / ENEMIES_PER_ROW);
    enemy->destroyed = false;
    enemy->value = MAKE_BCD(100);
    enemy->bullet_cooldown = 20;
    enemy->bullet_timer = 0;
    move_sprite(enemy->sprite_index, enemy->location[0], enemy->location[1]);

    switch (enemy_groups[enemy_stage % ENEMY_GROUPS_COUNT][i])
    {
    case 1:
      enemy->sprite_top_offset = 0;
      enemy->sprite_bottom_offset = 1;
      enemy->sprite_left_offset = 0;
      enemy->sprite_right_offset = 0;
      enemy->top_enemy = true;
      enemy->bottom_enemy = true;
      enemy->tile_index = ENEMY_MULTI_TILE_INDEX;
      set_sprite_tile(enemy->sprite_index, ENEMY_MULTI_TILE_INDEX);
      enemies_remaining += 2;
      break;
    case 2:
      enemy->sprite_top_offset = 0;
      enemy->sprite_bottom_offset = 1 + HALF_SPRITE_HEIGHT;
      enemy->sprite_left_offset = 0;
      enemy->sprite_right_offset = 0;
      enemy->top_enemy = true;
      enemy->bottom_enemy = false;
      enemy->tile_index = ENEMY_MULTI_TILE_INDEX + 2;
      set_sprite_tile(enemy->sprite_index, ENEMY_MULTI_TILE_INDEX + 2);
      enemies_remaining += 1;
      break;
    case 3:
      enemy->sprite_top_offset = HALF_SPRITE_HEIGHT;
      enemy->sprite_bottom_offset = 1;
      enemy->sprite_left_offset = 0;
      enemy->sprite_right_offset = 0;
      enemy->top_enemy = false;
      enemy->bottom_enemy = true;
      enemy->tile_index = ENEMY_MULTI_TILE_INDEX + 4;
      set_sprite_tile(enemy->sprite_index, ENEMY_MULTI_TILE_INDEX + 4);
      enemies_remaining += 1;
      break;
    case 4:
      enemy->sprite_top_offset = 0;
      enemy->sprite_bottom_offset = 1;
      enemy->sprite_left_offset = 5;
      enemy->sprite_right_offset = 0;
      enemy->top_enemy = true;
      enemy->bottom_enemy = true;
      enemy->tile_index = ENEMY_MULTI_TILE_INDEX + 6;
      set_sprite_tile(enemy->sprite_index, ENEMY_MULTI_TILE_INDEX + 6);
      enemies_remaining += 2;
      break;
    }

    // Add to correct screen area
    if (enemy->location[0] < HALF_SCREEN_WIDTH) {
      enemies_left[i] = 1;
      enemies_right[i] = 0;
    } else {
      enemies_right[i] = 1;
      enemies_left[i] = 0;
    }

    wait_vbl_done();
    enemy++;
  }
  prev_leftmost_enemy_x = enemies[0].location[0];
  prev_rightmost_enemy_x = enemies[ENEMY_ARRAY_LENGTH - 1].location[0] + SPRITE_WIDTH;
  lowest_enemy_y = 0;
}

void enemy_collision_check(uint8_t i) {
  // Only check if the bullet is on screen.
  if (player_bullet.location[1] + SPRITE_HEIGHT > 16) {
    // Only check collision for enemies on the same side of the screen as the bullet.
    if ((player_bullet.location[0] < HALF_SCREEN_WIDTH + SPRITE_WIDTH && enemies_left[i] == 1) || (player_bullet.location[0] >= HALF_SCREEN_WIDTH - SPRITE_WIDTH && enemies_right[i] == 1)) {
      // Check whether the enemy is being hit by the player's bullet.
      if (player_bullet.location[0] + player_bullet.sprite_left_offset < enemies[i].location[0] + enemies[i].sprite_right_offset + SPRITE_WIDTH &&
              player_bullet.location[0] - player_bullet.sprite_right_offset + SPRITE_WIDTH > enemies[i].location[0] - enemies[i].sprite_left_offset &&
              player_bullet.location[1] + player_bullet.sprite_top_offset < enemies[i].location[1] - enemies[i].sprite_bottom_offset + SPRITE_HEIGHT &&
              player_bullet.location[1] - player_bullet.sprite_bottom_offset + SPRITE_HEIGHT > enemies[i].location[1] + enemies[i].sprite_top_offset
      ) {
        // Set the explosion x location first, since we will move the destroyed enemies.
        explosion.location[0] = enemies[i].location[0] + (enemies[i].sprite_left_offset - enemies[i].sprite_right_offset) / 2;

        // Play explosion sound.
        set_sound(SOUND_EXPLOSION);

        // Top enemy hit.
        if (enemies[i].top_enemy && (player_bullet.location[1] + player_bullet.sprite_top_offset <= enemies[i].location[1] + HALF_SPRITE_HEIGHT)) {
          explosion.location[1] = enemies[i].location[1] + movement_y;
          // If the bottom enemy still exists, we update to only show that one.
          if (enemies[i].bottom_enemy) {
            enemies[i].top_enemy = false;
            enemies[i].sprite_top_offset = enemies[i].sprite_top_offset + 8;
            enemies[i].tile_index += 4;
            set_sprite_tile(enemies[i].sprite_index, enemies[i].tile_index);
          } else {
            // Enemy is totally destroyed.
            enemies[i].destroyed = true;
            enemies[i].location[0] = 0;
            enemies[i].location[1] = 0;
          }
        } else {
          // Bottom enemy hit.
          explosion.location[1] = enemies[i].location[1] + HALF_SPRITE_HEIGHT + movement_y;
          // If the top enemy still exists, we update to only show that one.
          if (enemies[i].top_enemy) {
            enemies[i].bottom_enemy = false;
            enemies[i].sprite_bottom_offset = enemies[i].sprite_bottom_offset + 8;
            enemies[i].tile_index += 2;
            set_sprite_tile(enemies[i].sprite_index, enemies[i].tile_index);
          } else {
            // Enemy is totally destroyed.
            enemies[i].destroyed = true;
            enemies[i].location[0] = 0;
            enemies[i].location[1] = 0;
          }
        }

        // Update the explosion to show on the enemies location.
        explosion.time_since_animation_start = 0;
        move_sprite(explosion.sprite_index, explosion.location[0], explosion.location[1]);
        explosion.is_on_screen = true;

        // Destroy the player's bullet.
        player_bullet.location[0] = 0;
        player_bullet.location[1] = 0;
        move_sprite(player_bullet.sprite_index, player_bullet.location[0], player_bullet.location[1]);

        // Update the score and enemy count.
        bcd_add(&score, &enemies[i].value);
        enemies_remaining -= 1;
      }
    }
  }
}

void prepare_move_enemies() {
  for (uint8_t i=0; i<ENEMY_ARRAY_LENGTH; i++) {
    // Already destroyed enemies don't need updating.
    if (!enemies[i].destroyed) {
      
      enemy_collision_check(i);

      // If the enemy is now destroyed, move it off screen and continue.
      if (enemies[i].destroyed) {
        move_sprite(enemies[i].sprite_index, 0, 0);
        continue;
      }

      // Prepare for enemy movement next frame.
      if (enemies[i].location[0] < cur_leftmost_enemy_x) {
        cur_leftmost_enemy_x = enemies[i].location[0];
      }

      if (enemies[i].location[0] + SPRITE_WIDTH > cur_rightmost_enemy_x) {
        cur_rightmost_enemy_x = enemies[i].location[0] + SPRITE_WIDTH;
      }

    }

    // Update bullet cooldown timer.
    if (enemies[i].bullet_timer > 0) {
      enemies[i].bullet_timer--;
    }
  }

  prev_rightmost_enemy_x = cur_rightmost_enemy_x;
  prev_leftmost_enemy_x = cur_leftmost_enemy_x;
  cur_leftmost_enemy_x = 255;
  cur_rightmost_enemy_x = 0;
}

void move_enemies() {
  // Determine movement for this turn.
  if (enemies_move_left) {
    if (prev_leftmost_enemy_x - 1 < 8) {
      movement_x = 0;
      movement_y = 4;
      enemies_move_down = true;
      enemies_move_left = false;
    } else {
      movement_x = -1;
      movement_y = 0;
    }
  } else {
    if (prev_rightmost_enemy_x + 1 > 168) {
      movement_x = 0;
      movement_y = 4;
      enemies_move_down = true;
      enemies_move_left = true;
    } else {
      movement_x = 1;
      movement_y = 0;
    }
  }

  for (uint8_t i=0; i<ENEMY_ARRAY_LENGTH; i++) {
    // Already destroyed enemies don't need updating.
    if (!enemies[i].destroyed) {
      
      // We do not do a collision check during the movement frame in order to save CPU time.
      // It won't be noticeable if it is one frame later while moving.

      // Move the enemy.
      enemies[i].location[0] = enemies[i].location[0] + movement_x;
      enemies[i].location[1] = enemies[i].location[1] + movement_y;

      move_sprite(enemies[i].sprite_index, enemies[i].location[0], enemies[i].location[1]);

      // Animate the enemy. The first and second frame are 64 tiles apart.
      enemies[i].tile_index ^= 64;
      set_sprite_tile(enemies[i].sprite_index, enemies[i].tile_index);

      // Determine if we are going to move from one half of the screen to the other.
      // Since all enemies always move by 1 pixel we can simply check for the dividing line.
      // For performance reasons we just update the left and right array whenever an enemy gets to the line.
      if (enemies[i].location[0] == (HALF_SCREEN_WIDTH - 1)) {
        enemies_left[i] = 1;
        enemies_right[i] = 0;
      } else if (enemies[i].location[0] == HALF_SCREEN_WIDTH) {
        enemies_left[i] = 0;
        enemies_right[i] = 1;
      }

    }
    
    // Update bullet cooldown timer.
    if (enemies[i].bullet_timer > 0) {
      enemies[i].bullet_timer--;
    }
  }

  if (enemies_move_down) {
    enemies_move_down = false;
    if (enemies_move_delay - enemies_move_delay_decrease <= ENEMY_MOVEMENT_DELAY_MIN) {
      enemies_move_delay = ENEMY_MOVEMENT_DELAY_MIN;
    } else {
      enemies_move_delay -= enemies_move_delay_decrease;
    }
  }
  
  set_sound(SOUND_ENEMIES_MOVE);
}

void after_move_enemies() {
  for (uint8_t i=0; i<ENEMY_ARRAY_LENGTH; i++) {
    // Already destroyed enemies don't need updating.
    if (!enemies[i].destroyed) {
      
      enemy_collision_check(i);

      // If the enemy is now destroyed, move it off screen and continue.
      if (enemies[i].destroyed) {
        move_sprite(enemies[i].sprite_index, 0, 0);
        continue;
      }

      // We do this right after moving, in order to spread the cpu load.
      // Determine the lower y coordinate of the lowest enemy on screen.
      if (enemies[i].bottom_enemy) {
        if (enemies[i].location[1] + SPRITE_HEIGHT > lowest_enemy_y) {
          lowest_enemy_y = enemies[i].location[1] + SPRITE_HEIGHT;
        }
      } else {
        if (enemies[i].location[1] + (SPRITE_HEIGHT / 2) > lowest_enemy_y) {
          lowest_enemy_y = enemies[i].location[1] + (SPRITE_HEIGHT / 2);
        }
      }

    }

    // Update bullet cooldown timer.
    if (enemies[i].bullet_timer > 0) {
      enemies[i].bullet_timer--;
    }
  }
}

void regular_enemies_update() {
  for (uint8_t i=0; i<ENEMY_ARRAY_LENGTH; i++) {
    // Already destroyed enemies don't need updating.
    if (!enemies[i].destroyed) {
      
      enemy_collision_check(i);

      // If the enemy is now destroyed, move it off screen.
      if (enemies[i].destroyed) {
        move_sprite(enemies[i].sprite_index, 0, 0);
      }

    }

    // Update bullet cooldown timer.
    if (enemies[i].bullet_timer > 0) {
      enemies[i].bullet_timer--;
    }
  }
}

void update_enemies() {
  if (enemies_remaining == 0) {
    if (next_wave_timer < 10) {
      next_wave_timer++;
    } else {
      next_wave_timer = 0;
      enemy_stage += 1;
      if (enemies_move_delay - enemies_move_delay_decrease <= ENEMY_MOVEMENT_DELAY_MIN) {
        enemies_move_delay = ENEMY_MOVEMENT_DELAY_MIN;
      } else {
        enemies_move_delay -= enemies_move_delay_decrease;
      }
      
      if (enemy_bullets_cooldown > ENEMY_BULLETS_DECREASE) {
        enemy_bullets_cooldown -= ENEMY_BULLETS_DECREASE;
      }
      createEnemies();
    }
  }

  if (enemy_movement_timer == enemies_move_delay - 1) {
    prepare_move_enemies();
    enemy_movement_timer++;
  } else if (enemy_movement_timer == 0) {
    after_move_enemies();

    // Lose condition when an enemy reaches the height of the player
    if (lowest_enemy_y >= PLAYER_START_HEIGHT) {
      game_over();
    }

    enemy_movement_timer++;
  } else if (enemy_movement_timer == enemies_move_delay) {
    move_enemies();
    enemy_movement_timer = 0;
  } else {
    regular_enemies_update();
    enemy_movement_timer++;
  }

  
  // Check if it is time to shoot and there is a bullet slot free.
  if (enemy_bullets_timer >= enemy_bullets_cooldown) {
    if (enemy_bullets_count < ENEMY_BULLETS_ARRAY_LENGTH) {
      // Determine which enemy will fire.
      enemy_which_will_shoot = rand();
      enemy_which_will_shoot = enemy_which_will_shoot % ENEMY_ARRAY_LENGTH;

      // If this enemy has already been destroyed, keep adding 1 until we find one that exists.
      do {
        enemy_which_will_shoot++;
        if (enemy_which_will_shoot >= ENEMY_ARRAY_LENGTH) {
          enemy_which_will_shoot = 0;
        }
      } while (enemies[enemy_which_will_shoot].destroyed);

      // If a lower enemy exists, that enemy will shoot.
      do {
        if (!enemies[enemy_which_will_shoot + ENEMIES_PER_ROW].destroyed) {
          enemy_which_will_shoot += ENEMIES_PER_ROW;
        } else {
          break;
        }
      } while (enemy_which_will_shoot + ENEMIES_PER_ROW < ENEMY_ARRAY_LENGTH);

      // If the enemy bullet cooldown is not yet done, we can skip it.
      if (enemies[enemy_which_will_shoot].bullet_timer == 0) {
        for (uint8_t j = 0; j < ENEMY_BULLETS_ARRAY_LENGTH; j++) {
          if (enemy_bullets[j].location[1] + SPRITE_HEIGHT <= 16 || enemy_bullets[j].location[1] >= 160) {
            enemy_bullets[j].location[0] = enemies[enemy_which_will_shoot].location[0] + enemies[enemy_which_will_shoot].sprite_left_offset - enemies[enemy_which_will_shoot].sprite_right_offset;
            enemy_bullets[j].location[1] = enemies[enemy_which_will_shoot].location[1] + SPRITE_HEIGHT - enemies[enemy_which_will_shoot].sprite_bottom_offset;
            enemy_bullets[j].speed = 1;
            enemy_bullets[j].sprite_left_offset = 3;
            enemy_bullets[j].sprite_right_offset = 3;
            enemy_bullets[j].sprite_top_offset = 0;
            enemy_bullets[j].sprite_bottom_offset = 8;
            set_sprite_tile(enemy_bullets[j].sprite_index, ENEMY_BULLETS_TILE_INDEX);
            move_sprite(enemy_bullets[j].sprite_index, enemy_bullets[j].location[0], enemy_bullets[j].location[1]);

            enemy_bullets_count++;
            enemies[enemy_which_will_shoot].bullet_timer = enemies[enemy_which_will_shoot].bullet_cooldown;

            // Once one bullet has been fired, we can jump out of the loop.
            break;
          }
        }
      }
    }
    enemy_bullets_timer = 0;
  } else {
    enemy_bullets_timer++;
  }
}

void update_enemy_bullets() {
  for (uint8_t i=0; i<ENEMY_BULLETS_ARRAY_LENGTH; i++) {
    enemy_bullets_count = 0;
    if (enemy_bullets[i].location[1] < 160 && enemy_bullets[i].location[1] + SPRITE_HEIGHT > 16) {
      enemy_bullets[i].location[1] = enemy_bullets[i].location[1] + 1 * enemy_bullets[i].speed;
      move_sprite(enemy_bullets[i].sprite_index, enemy_bullets[i].location[0], enemy_bullets[i].location[1]);
      enemy_bullets_count++;

      // Check whether the player is being hit by an enemy bullet.
      if (enemy_bullets[i].location[1] + SPRITE_HEIGHT - enemy_bullets[i].sprite_bottom_offset >= PLAYER_START_HEIGHT) {
        if (enemy_bullets[i].location[0] + enemy_bullets[i].sprite_left_offset < player.location[0] + SPRITE_WIDTH * 2 &&
                enemy_bullets[i].location[0] - enemy_bullets[i].sprite_right_offset + SPRITE_WIDTH > player.location[0] &&
                enemy_bullets[i].location[1] + enemy_bullets[i].sprite_top_offset < player.location[1] + SPRITE_HEIGHT &&
                enemy_bullets[i].location[1] - enemy_bullets[i].sprite_bottom_offset + SPRITE_HEIGHT > player.location[1] + 4
        ) {
          game_over();
        }
      }

      // Check whether the player's bullet is hit by an enemy bullet.
      if (enemy_bullets[i].location[0] + enemy_bullets[i].sprite_left_offset < player_bullet.location[0] + SPRITE_WIDTH - player_bullet.sprite_right_offset &&
              enemy_bullets[i].location[0] - enemy_bullets[i].sprite_right_offset + SPRITE_WIDTH > player_bullet.location[0] + player_bullet.sprite_left_offset &&
              enemy_bullets[i].location[1] + enemy_bullets[i].sprite_top_offset < player_bullet.location[1] + SPRITE_HEIGHT - player_bullet.sprite_bottom_offset &&
              enemy_bullets[i].location[1] - enemy_bullets[i].sprite_bottom_offset + SPRITE_HEIGHT > player_bullet.location[1] + player_bullet.sprite_top_offset
      ) {
        // Update the explosion to show on the bullet location.
        explosion.location[0] = enemy_bullets[i].location[0];
        explosion.location[1] = enemy_bullets[i].location[1];
        explosion.time_since_animation_start = 0;
        move_sprite(explosion.sprite_index, explosion.location[0], explosion.location[1]);
        explosion.is_on_screen = true;

        // Destroy the player's bullet and enemy bullet.
        player_bullet.location[0] = 0;
        player_bullet.location[1] = 0;
        move_sprite(player_bullet.sprite_index, 0, 0);
        enemy_bullets[i].location[0] = 0;
        enemy_bullets[i].location[1] = 0;
        move_sprite(enemy_bullets[i].sprite_index, 0, 0);
        enemy_bullets_count--;
      }
    }
  }
}

/* 
  ###############
  END ENEMY LOGIC
  ###############
*/

void update_window() {
  unsigned char buffer[9];
  bcd2text(&score, 0x01, buffer);
  set_win_tiles(6, 0, 8, 1, buffer);
}


void update_controls() {
  // Player controls
  if (player.can_move) {
    if (KEY_DEBOUNCE(J_LEFT)) {
      if (player.location[0] - 1 >= 8) {
        player.location[0]--;
        move_player();
        player.can_move = false;
      }
    }
    if (KEY_DEBOUNCE(J_RIGHT)) {
      if (player.location[0] + SPRITE_WIDTH * 2 + 1 <= 168) {
        player.location[0]++;
        move_player();
        player.can_move = false;
      }
    }
  }
  if (player.can_shoot) {
    if (KEY_TICKED(J_A)) {
      player_bullet.location[0] = player.location[0] + SPRITE_WIDTH / 2;
      player_bullet.location[1] = player.location[1] - 3;
      move_sprite(player_bullet.sprite_index, player_bullet.location[0], player_bullet.location[1]);
      player.can_shoot = false;
      set_sound(SOUND_PLAYER_SHOOT);
    }
  }
}

void init_game() {
  set_music(MUSIC_STAGE1);

  score = MAKE_BCD(0);

  // Create and set random seed.
  seed = DIV_REG;
  initrand(seed);

  // Create the score text on screen.
  unsigned char scoreText[] = {0x1D, 0xD, 0x19, 0x1C, 0xF};
  set_win_tiles(0, 0, 5, 1, scoreText);

  enemies_remaining = 0;
  enemy_movement_timer = 0;
  enemy_stage = 0;
  next_wave_timer = 0;
  cur_leftmost_enemy_x = 255;
  cur_rightmost_enemy_x = 0;
  lowest_enemy_y = 0;
  movement_x = 0;
  movement_y = 0;
  enemies_move_left = false;
  enemies_move_down = false;
  enemies_move_delay = ENEMY_MOVEMENT_DELAY;
  enemies_move_delay_decrease = ENEMY_MOVEMENT_DELAY_DECREASE;
  enemy_bullets_cooldown = ENEMY_BULLETS_COOLDOWN;
 
  for (int8_t i = 0; i < ENEMY_BULLETS_ARRAY_LENGTH; i++) {
    enemy_bullets[i].sprite_index = ENEMY_BULLETS_SPRITE_INDEX + i; 
  }

  // Set initial values of the player instance.
  player.sprite_count = 2;
  player.sprite_index = 0;
  player.location[0] = PLAYER_START_WIDTH;
  player.location[1] = PLAYER_START_HEIGHT;
  player.time_since_animation_start = 0;
  player.time_since_last_moved = 0;
  player.move_speed_delay = 0;
  player.can_move = true;
  player.can_shoot = true;
  // Set player sprite data.
  set_sprite_data(PLAYER_TILE_INDEX, player.sprite_count * 2, player_sprites);
  set_sprite_prop(player.sprite_index + 1, S_FLIPX);
  move_player();

  // Set initial values of the player bullet instance.
  player_bullet.sprite_count = 1;
  player_bullet.sprite_index = 2;
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
  move_sprite(player_bullet.sprite_index, player_bullet.location[0], player_bullet.location[1]);

  // Set enemy sprite data.
  set_sprite_data(ENEMY_MULTI_TILE_INDEX, 3 * ENEMY_TYPES * TILE_INDEX_MULTIPLIER, enemy_sprites);
  set_sprite_data(ENEMY_SECOND_FRAME_TILE_INDEX, 3 * ENEMY_TYPES * TILE_INDEX_MULTIPLIER, enemy_sprites_sec);
  // Set initial values of enemy instances.
  createEnemies();

  // Set initial values of the explosion instance.
  explosion.sprite_index = 39;
  explosion.location[0] = 0;
  explosion.location[1] = 0;
  explosion.is_on_screen = false;
  // Set explostion sprite data.
  set_sprite_data(EXPLOSION_TILE_INDEX, 1 * TILE_INDEX_MULTIPLIER, explosion_sprites);
  set_sprite_tile(explosion.sprite_index, EXPLOSION_TILE_INDEX);
  move_sprite(explosion.sprite_index, explosion.location[0], explosion.location[1]);

  SPRITES_8x16;
  SHOW_SPRITES;
  SHOW_WIN;
}

void run_game() {
  UPDATE_KEYS();
  update_controls();  

  // If music and/or sound are set, play them.
  // play_music();
  stop_music();
  play_sound();

  // Update everything for the player.
  update_player();
  animate_player();

  update_enemies();
  update_enemy_bullets();

  update_explosion();
  
  update_player_bullet();
  
  update_window();

  wait_vbl_done();
}