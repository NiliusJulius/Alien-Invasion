#include <gb/gb.h>
#include <gb/font.h>
#include <stdio.h>
#include "bcd.h"
#include "typedefs.c"
#include "enemygroups.c"
#include "sprites\PlayerSprites.h"
#include "sprites\BulletSprites.h"
#include "sprites\Explosion.h"
#include "sprites\MultipleEnemySprites.h"
#include "maps\WindowMap.h"

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

Player player;
Bullet playerBullet;
Enemy enemies[ENEMY_ARRAY_LENGTH];
Explosion explosion;

void performantdelay(UINT8 numloops){
    UINT8 i;
    for(i = 0; i < numloops; i++){
        wait_vbl_done();
    }     
}

void animatePlayer() {
  UINT8 currentFrame;
  if (player.timeSinceAnimationStart >= player.spriteCount * PLAYER_ANIM_SPEED) {
    // If we passed the last frame time, reset the timer.
    player.timeSinceAnimationStart = 0;
    currentFrame = 0;
  } else {
    // else find the frame we should be at.
    currentFrame = player.timeSinceAnimationStart / PLAYER_ANIM_SPEED;
  }

  set_sprite_tile(player.spriteIndex, currentFrame * TILE_INDEX_MULTIPLIER);
  player.timeSinceAnimationStart = player.timeSinceAnimationStart + 1;
}

void updateExplosion() {
  if (explosion.isOnScreen) {
    if (explosion.timeSinceAnimationStart >= 6) {
      explosion.location[0] = 0;
      explosion.location[1] = 0;
      move_sprite(explosion.spriteIndex, explosion.location[0], explosion.location[1]);
      explosion.isOnScreen = FALSE;
    }
    explosion.timeSinceAnimationStart = explosion.timeSinceAnimationStart + 1;
  }
}

void updatePlayer() {
  if (player.timeSinceLastMoved >= player.moveSpeedDelay) {
    player.canMove = TRUE;
    player.timeSinceLastMoved = 0;
  } else if (!player.canMove) {
    player.timeSinceLastMoved++;
  }
}

void updatePlayerBullet() {
  if (playerBullet.location[1] + SPRITE_HEIGHT > 16 && playerBullet.location[1] < 160) {
    playerBullet.location[1] = playerBullet.location[1] - 1 * playerBullet.speed;
    move_sprite(playerBullet.spriteIndex, playerBullet.location[0], playerBullet.location[1]);
  } else {
    player.canShoot = TRUE;
  }
}

void updateEnemies() {
  for (UINT8 i=0; i<ENEMY_ARRAY_LENGTH; i++) {
    // Already destroyed enemies don't need updating.
    if (!enemies[i].destroyed) {
      // Check whether the enemy is being hit by the player's bullet.
      if (playerBullet.location[0] + playerBullet.spriteLeftOffset < enemies[i].location[0] + enemies[i].spriteRightOffset + SPRITE_WIDTH &&
              playerBullet.location[0] - playerBullet.spriteRightOffset + SPRITE_WIDTH > enemies[i].location[0] - enemies[i].spriteLeftOffset &&
              playerBullet.location[1] + playerBullet.spriteTopOffset < enemies[i].location[1] - enemies[i].spriteBottomOffset + SPRITE_HEIGHT &&
              playerBullet.location[1] - playerBullet.spriteBottomOffset + SPRITE_HEIGHT > enemies[i].location[1] + enemies[i].spriteTopOffset
      ) {
        // Set the explosion x location first, since we will move the destroyed enemies.
        explosion.location[0] = enemies[i].location[0];

        // Top enemy hit.
        if ((playerBullet.location[1] + playerBullet.spriteTopOffset <= enemies[i].location[1] + HALF_SPRITE_HEIGHT)) {
          explosion.location[1] = enemies[i].location[1];
          // If the bottom enemy still exists, we update to only show that one.
          if (enemies[i].bottomEnemy) {
            enemies[i].topEnemy = FALSE;
            enemies[i].spriteTopOffset = 8;
            set_sprite_tile(enemies[i].spriteIndex, ENEMY_MULTI_TILE_INDEX + 4);
          } else {
            // Enemy is totally destroyed.
            enemies[i].destroyed = TRUE;
            enemies[i].location[0] = 0;
            enemies[i].location[1] = 0;
            move_sprite(enemies[i].spriteIndex, 0, 0);
          }
        } else {
          // Bottom enemy hit.
          explosion.location[1] = enemies[i].location[1] + HALF_SPRITE_HEIGHT;
          // If the top enemy still exists, we update to only show that one.
          if (enemies[i].topEnemy) {
            enemies[i].bottomEnemy = FALSE;
            enemies[i].spriteBottomOffset = 1 + 8;
            set_sprite_tile(enemies[i].spriteIndex, ENEMY_MULTI_TILE_INDEX + 2);
          } else {
            // Enemy is totally destroyed.
            enemies[i].destroyed = TRUE;
            enemies[i].location[0] = 0;
            enemies[i].location[1] = 0;
            move_sprite(enemies[i].spriteIndex, 0, 0);
          }
        }

        // Update the explosion to show on the enemies location.
        explosion.timeSinceAnimationStart = 0;
        move_sprite(explosion.spriteIndex, explosion.location[0], explosion.location[1]);
        explosion.isOnScreen = TRUE;

        // Destroy the player's bullet.
        playerBullet.location[0] = 0;
        playerBullet.location[1] = 1;
        move_sprite(playerBullet.spriteIndex, playerBullet.location[0], playerBullet.location[1]);

        // Update the score.
        bcd_add(&score, &enemies[i].value);
      }
    }
  }
}

void updateWindow() {
  unsigned char buffer[9];
  bcd2text(&score, 0x01, buffer);
  set_win_tiles(6, 0, 8, 1, buffer);
}

void createEnemies() {
  Enemy *enemy = enemies;
  for (UINT8 i=0; i<ENEMY_ARRAY_LENGTH; i++) {
    enemy->spriteIndex = 15 + i;
    enemy->location[0] = 24 + SPRITE_WIDTH * 2 * (i % 8);
    enemy->location[1] = 30 + SPRITE_HEIGHT * (i / 8);
    enemy->destroyed = FALSE;
    enemy->value = MAKE_BCD(100);
    move_sprite(enemy->spriteIndex, enemy->location[0], enemy->location[1]);

    switch (enemyGroup1[i])
    {
    case 1:
      enemy->spriteTopOffset = 0;
      enemy->spriteBottomOffset = 1;
      enemy->spriteLeftOffset = 0;
      enemy->spriteRightOffset = 0;
      enemy->topEnemy = TRUE;
      enemy->bottomEnemy = TRUE;
      set_sprite_tile(enemy->spriteIndex, ENEMY_MULTI_TILE_INDEX);
      break;
    case 2:
      enemy->spriteTopOffset = 0;
      enemy->spriteBottomOffset = 1 + HALF_SPRITE_HEIGHT;
      enemy->spriteLeftOffset = 0;
      enemy->spriteRightOffset = 0;
      enemy->topEnemy = TRUE;
      enemy->bottomEnemy = FALSE;
      set_sprite_tile(enemy->spriteIndex, ENEMY_MULTI_TILE_INDEX + 2);
      break;
    case 3:
      enemy->spriteTopOffset = HALF_SPRITE_HEIGHT;
      enemy->spriteBottomOffset = 1;
      enemy->spriteLeftOffset = 0;
      enemy->spriteRightOffset = 0;
      enemy->topEnemy = FALSE;
      enemy->bottomEnemy = TRUE;
      set_sprite_tile(enemy->spriteIndex, ENEMY_MULTI_TILE_INDEX + 4);
      break;    
    default:
      break;
    }

    enemy++;
  }
}

void main() {
  SPRITES_8x16;
  SHOW_SPRITES;
  SHOW_WIN;

  // Create the score text
  unsigned char scoreText[] = {0x1D, 0xD, 0x19, 0x1C, 0xF};
  set_win_tiles(0, 0, 5, 1, scoreText);

  font_t min_font;
  font_init();
  min_font = font_load(font_min);
  font_set(min_font);

  // Set initial values of the player instance.
  player.spriteCount = 2;
  player.spriteIndex = 0;
  player.location[0] = 30;
  player.location[1] = 140;
  player.timeSinceAnimationStart = 0;
  player.timeSinceLastMoved = 0;
  player.moveSpeedDelay = 0;
  player.canMove = TRUE;
  player.canShoot = TRUE;
  // Set player sprite data.
  set_sprite_data(PLAYER_TILE_INDEX, player.spriteCount * 2, PlayerSprites);
  move_sprite(player.spriteIndex, player.location[0], player.location[1]);

  // Set initial values of the player bullet instance.
  playerBullet.spriteCount = 1;
  playerBullet.spriteIndex = 1;
  playerBullet.location[0] = 0;
  playerBullet.location[1] = 0;
  playerBullet.speed = 3;
  playerBullet.spriteLeftOffset = 3;
  playerBullet.spriteRightOffset = 3;
  playerBullet.spriteTopOffset = 0;
  playerBullet.spriteBottomOffset = 8;
  // Set player bullet sprite data.
  set_sprite_data(PLAYER_BULLET_TILE_INDEX, playerBullet.spriteCount * TILE_INDEX_MULTIPLIER, BulletSprites);
  set_sprite_tile(playerBullet.spriteIndex, PLAYER_BULLET_TILE_INDEX);

  // Set initial values of enemy instances.
  createEnemies();
  // Set enemy sprite data.
  set_sprite_data(ENEMY_MULTI_TILE_INDEX, 3 * TILE_INDEX_MULTIPLIER, MultipleEnemySprites);

  // Set initial values of the explosion instance.
  explosion.spriteIndex = 39;
  explosion.location[0] = 0;
  explosion.location[1] = 0;
  explosion.isOnScreen = FALSE;
  // Set explostion sprite data.
  set_sprite_data(EXPLOSION_TILE_INDEX, 1 * TILE_INDEX_MULTIPLIER, ExplosionSprite);
  set_sprite_tile(explosion.spriteIndex, EXPLOSION_TILE_INDEX);

  // Game loop.
  while (1) {
    
    // Player controls
    if (player.canMove) {
      if (joypad()&J_LEFT) {
        if (player.location[0] - 1 >= 8) {
          player.location[0]--;
          move_sprite(player.spriteIndex, player.location[0], player.location[1]);
          player.canMove = FALSE;
        }
      }
      if (joypad()&J_RIGHT) {
        if (player.location[0] + SPRITE_WIDTH + 1 <= 168) {
          player.location[0]++;
          move_sprite(player.spriteIndex, player.location[0], player.location[1]);
          player.canMove = FALSE;
        }
      }
    }
    if (joypad()&J_A) {
      if (player.canShoot) {
        playerBullet.location[0] = player.location[0];
        playerBullet.location[1] = player.location[1] - 1;
        move_sprite(playerBullet.spriteIndex, playerBullet.location[0], playerBullet.location[1]);
        player.canShoot = FALSE;
      }
    }

    // Update everything for the player.
    updatePlayer();
    animatePlayer();

    // Update everything with enemies.
    updateEnemies();

    updateExplosion();
    
    updatePlayerBullet();
    
    updateWindow();

    performantdelay(1);
  }
  
}