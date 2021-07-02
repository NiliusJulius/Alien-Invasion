#include <gb/gb.h>
#include <gb/font.h>
#include <stdio.h>
#include "sprites\PlayerSprites.h"
#include "sprites\BulletSprites.h"
#include "sprites\Explosion.h"
#include "sprites\MultipleEnemySprites.h"
#include "maps\WindowMap.h"

#define PLAYER_TILE_INDEX 0
#define PLAYER_BULLET_TILE_INDEX 4
#define EXPLOSION_TILE_INDEX 6
#define ENEMY_MULTI_TILE_INDEX 8

#define SPRITE_WIDTH 8
#define SPRITE_HEIGHT 16

// Because we are running in 8x16 sprite mode, all tile indexes should be multiplied by 2.
#define TILE_INDEX_MULTIPLIER 2

UINT16 score = 0;

// Player
typedef struct Player {
  UINT8 spriteCount;
  UINT8 spriteIndex;
  UINT8 location[2];
  UINT8 timeSinceAnimationStart;
  UINT8 timeSinceLastMoved;
  UINT8 moveSpeedDelay;
  BOOLEAN canMove;
  BOOLEAN canShoot;
} Player;

// Player bullet
typedef struct Bullet {
  UINT8 spriteCount;
  UINT8 spriteIndex;
  UINT8 location[2];
  UINT8 speed;
  UINT8 spriteLeftOffset;
  UINT8 spriteRightOffset;
  UINT8 spriteTopOffset;
  UINT8 spriteBottomOffset;
} Bullet;

typedef struct Enemy {
  UINT8 spriteIndex;
  UINT8 location[2];
  UINT8 spriteLeftOffset;
  UINT8 spriteRightOffset;
  UINT8 spriteTopOffset;
  UINT8 spriteBottomOffset;
  BOOLEAN topEnemy;
  BOOLEAN bottomEnemy;
  BOOLEAN bottomShot;
  BOOLEAN topShot;
} Enemy;

typedef struct Explosion {
  UINT8 spriteIndex;
  UINT8 location[2];
  UINT8 timeSinceAnimationStart;
} Explosion;

Player player;
Bullet playerBullet;
Enemy enemy;
Explosion explosion;


void performantdelay(UINT8 numloops){
    UINT8 i;
    for(i = 0; i < numloops; i++){
        wait_vbl_done();
    }     
}

BOOLEAN isOnScreen(UINT8 location[], INT8 width, INT8 height) {
  if (location[1] + height <= 16 || location[1] >= 148 || location[0] + width <= 8 || location[0] >= 168) {
    return FALSE;
  }
  return TRUE;
}

void animateSprite(UINT8 spriteIndex, UINT8 frameCount, UINT8 animationSpeed, UINT8 *timeSinceAnimationStart) {
  UINT8 currentFrame;
  if (*timeSinceAnimationStart >= frameCount * animationSpeed) {
    // If we passed the last frame time, reset the timer.
    *timeSinceAnimationStart = 0;
    currentFrame = 0;
  } else {
    // else find the frame we should be at.
    currentFrame = *timeSinceAnimationStart / animationSpeed;
  }

  set_sprite_tile(spriteIndex, currentFrame * TILE_INDEX_MULTIPLIER);
  *timeSinceAnimationStart = *timeSinceAnimationStart + 1;
}

BOOLEAN overlaps(UINT8 location1[], UINT8 leftOffset1, UINT8 rightOffset1, UINT8 topOffset1, UINT8 bottomOffset1, UINT8 location2[], UINT8 leftOffset2, UINT8 rightOffset2, UINT8 topOffset2, UINT8 bottomOffset2) {
  // TODO remove, only here for debug purposes.
  UINT8 x1 = location1[0];
  UINT8 x2 = location2[0];
  UINT8 y1 = location1[1];
  UINT8 y2 = location2[1];
  if (x1 + leftOffset1 < x2 + rightOffset2 + SPRITE_WIDTH &&
    x1 - rightOffset1 + SPRITE_WIDTH > x2 - leftOffset2 &&
    y1 + topOffset1 < y2 - bottomOffset2 + SPRITE_HEIGHT &&
    y1 - bottomOffset1 + SPRITE_HEIGHT > y2 + topOffset2) {
      return TRUE;
    }
  return FALSE;
}

void updateExplosion() {
  if (isOnScreen(explosion.location, SPRITE_WIDTH, SPRITE_HEIGHT / 2)) {
    if (explosion.timeSinceAnimationStart >= 6) {
      explosion.location[0] = 0;
      explosion.location[1] = 0;
      move_sprite(explosion.spriteIndex, explosion.location[0], explosion.location[1]);
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

void moveSprite(UINT8 spriteIndex, UINT8 currentLocation[], INT8 xChange, INT8 yChange) {
  currentLocation[0] = currentLocation[0] + xChange;
  currentLocation[1] = currentLocation[1] + yChange;
  move_sprite(spriteIndex, currentLocation[0], currentLocation[1]);
}

void hideSprite(UINT8 spriteIndex, UINT8 currentLocation[]) {
  currentLocation[0] = 0;
  currentLocation[1] = 0;
  move_sprite(spriteIndex, currentLocation[0], currentLocation[1]);
}

void enemyHit(Enemy *enemy) {
  explosion.timeSinceAnimationStart = 0;
  explosion.location[0] = enemy->location[0];

  if (playerBullet.location[1] + playerBullet.spriteTopOffset <= enemy->location[1] + (SPRITE_HEIGHT / 2)) {
    enemy->topEnemy = FALSE;
    explosion.location[1] = enemy->location[1];
    enemy->spriteTopOffset = 8;
  } else {
    enemy->bottomEnemy = FALSE;
    explosion.location[1] = enemy->location[1] + (SPRITE_HEIGHT / 2);
    enemy->spriteBottomOffset = 8;
  }

  move_sprite(explosion.spriteIndex, explosion.location[0], explosion.location[1]);
}

void updatePlayerBullet(Enemy *enemy) {
  if (isOnScreen(playerBullet.location, SPRITE_WIDTH - playerBullet.spriteLeftOffset - playerBullet.spriteRightOffset, SPRITE_HEIGHT - playerBullet.spriteTopOffset - playerBullet.spriteBottomOffset)) {
    if (overlaps(playerBullet.location, playerBullet.spriteLeftOffset, playerBullet.spriteRightOffset, playerBullet.spriteTopOffset, playerBullet.spriteBottomOffset, 
      enemy->location, enemy->spriteLeftOffset, enemy->spriteRightOffset, enemy->spriteTopOffset, enemy->spriteBottomOffset)
    ) {
      enemyHit(enemy);
      hideSprite(playerBullet.spriteIndex, playerBullet.location);
      score = score + 100;
    }

    moveSprite(playerBullet.spriteIndex, playerBullet.location, 0, -1 * playerBullet.speed);
  } else {
    player.canShoot = TRUE;
  }
}

void updateWindow() {
  UINT16 remainingScore = score;
  UINT8 digitsDrawn = 0;
  do {
    UINT8 digit = remainingScore % 10 + 1;
    set_win_tiles(19 - digitsDrawn, 17, 1, 1, &digit);
    digitsDrawn++;
    remainingScore = remainingScore / 10;
  } while (remainingScore > 0);
}

void updateEnemy(Enemy *enemy) {
  UINT8 tileOffset = 0;
  if (!enemy->bottomEnemy && enemy->topEnemy) {
    // Only the top enemy is left.
    tileOffset = 1;
  } else if (!enemy->topEnemy && enemy->bottomEnemy) {
    // Only the bottom enemy is left.
    tileOffset = 2;
  } else if (!enemy->topEnemy && !enemy->bottomEnemy) {
    // Both enemies have been destroyed.
    enemy->location[0] = 0;
    enemy->location[1] = 0;
  }
  set_sprite_tile(enemy->spriteIndex, ENEMY_MULTI_TILE_INDEX + tileOffset * TILE_INDEX_MULTIPLIER);
  move_sprite(enemy->spriteIndex, enemy->location[0], enemy->location[1]);
}

void main() {
  SPRITES_8x16;
  SHOW_SPRITES;
  SHOW_WIN;

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
  playerBullet.speed = 2;
  playerBullet.spriteLeftOffset = 3;
  playerBullet.spriteRightOffset = 3;
  playerBullet.spriteTopOffset = 0;
  playerBullet.spriteBottomOffset = 8;
  // Set player bullet sprite data.
  set_sprite_data(PLAYER_BULLET_TILE_INDEX, playerBullet.spriteCount * TILE_INDEX_MULTIPLIER, BulletSprites);
  set_sprite_tile(playerBullet.spriteIndex, PLAYER_BULLET_TILE_INDEX);

  // Set initial values of an instance of enemy.
  enemy.spriteIndex = 2;
  enemy.location[0] = 30;
  enemy.location[1] = 30;
  enemy.spriteLeftOffset = 0;
  enemy.spriteRightOffset = 0;
  enemy.spriteTopOffset = 0;
  enemy.spriteBottomOffset = 0;
  enemy.topEnemy = TRUE;
  enemy.bottomEnemy = TRUE;
  enemy.bottomShot = FALSE;
  enemy.topShot = FALSE;
  // Set enemy sprite data.
  set_sprite_data(ENEMY_MULTI_TILE_INDEX, 3 * TILE_INDEX_MULTIPLIER, MultipleEnemySprites);
  set_sprite_tile(enemy.spriteIndex, ENEMY_MULTI_TILE_INDEX);
  move_sprite(enemy.spriteIndex, enemy.location[0], enemy.location[1]);

  // Set initial values of the explosion instance.
  explosion.spriteIndex = 39;
  explosion.location[0] = 0;
  explosion.location[1] = 0;
  // Set explostion sprite data.
  set_sprite_data(EXPLOSION_TILE_INDEX, 1 * TILE_INDEX_MULTIPLIER, ExplosionSprite);
  set_sprite_tile(explosion.spriteIndex, EXPLOSION_TILE_INDEX);

  // Game loop.
  while (1) {

    // Player controls
    if (player.canMove) {
      if (joypad()&J_LEFT) {
        if (player.location[0] - 1 >= 8) {
          moveSprite(player.spriteIndex, player.location, -1, 0);
          player.canMove = FALSE;
        }
      }
      if (joypad()&J_RIGHT) {
        if (player.location[0] + SPRITE_WIDTH + 1 <= 168) {
          moveSprite(player.spriteIndex, player.location, 1, 0);
          player.canMove = FALSE;
        }
      }
    }
    if (joypad()&J_A) {
      if (player.canShoot) {
        playerBullet.location[0] = player.location[0];
        playerBullet.location[1] = player.location[1] - 8;
        moveSprite(playerBullet.spriteIndex, playerBullet.location, 0, 0);
        player.canShoot = FALSE;
      }
    }
        
    updateWindow();
    updatePlayer();
    updatePlayerBullet(&enemy);
    // Animate the player.
    animateSprite(player.spriteIndex, player.spriteCount, 7, &player.timeSinceAnimationStart);
    updateEnemy(&enemy);
    updateExplosion();
    performantdelay(1);
  }
  
}