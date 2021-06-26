#include <gb/gb.h>
#include <gb/font.h>
#include <stdio.h>
#include "sprites\PlayerSprites.h"
#include "sprites\BulletSprites.h"
#include "sprites\EnemySprites.h"
#include "sprites\MultipleEnemySprites.h"
#include "maps\WindowMap.h"

#define PLAYER_TILE_INDEX 0
#define PLAYER_BULLET_TILE_INDEX 4
#define ENEMY_TILE_INDEX 6

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
} Enemy;

void performantdelay(UINT8 numloops){
    UINT8 i;
    for(i = 0; i < numloops; i++){
        wait_vbl_done();
    }     
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

BOOLEAN isOnScreen(UINT8 location[], UINT8 width, UINT8 height) {
  if (location[1] + height <= 16 || location[1] >= 148 || location[0] + width <= 8 || location[0] >= 174) {
    return FALSE;
  }
  return TRUE;
}

void updatePlayer(Player *player) {
  if (player->timeSinceLastMoved >= player->moveSpeedDelay) {
    player->canMove = TRUE;
    player->timeSinceLastMoved = 0;
  } else if (!player->canMove) {
    player->timeSinceLastMoved++;
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

void updatePlayerBullet(Player *player, Bullet *playerBullet, Enemy *enemy) {
  if (overlaps(playerBullet->location, playerBullet->spriteLeftOffset, playerBullet->spriteRightOffset, playerBullet->spriteTopOffset, playerBullet->spriteBottomOffset, enemy->location, enemy->spriteLeftOffset, enemy->spriteRightOffset, enemy->spriteTopOffset, enemy->spriteBottomOffset)) {
    hideSprite(playerBullet->spriteIndex, playerBullet->location);
    score = score + 100;
  }

  if (isOnScreen(playerBullet->location, 8, 8)) {
    moveSprite(playerBullet->spriteIndex, playerBullet->location, 0, -1 * playerBullet->speed);
  } else {
    player->canShoot = TRUE;
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

void drawEnemy(Enemy *enemy) {
  UINT8 tileOffset = 0;
  if (!enemy->bottomEnemy) {
    tileOffset = 3;
  } else if (!enemy->topEnemy) {
    tileOffset = 4;
  }
  set_sprite_tile(enemy->spriteIndex, ENEMY_TILE_INDEX + tileOffset * TILE_INDEX_MULTIPLIER);
}

void main() {
  SPRITES_8x16;
  SHOW_SPRITES;
  SHOW_WIN;

  font_t min_font;
  font_init();
  min_font = font_load(font_min);
  font_set(min_font);

  // Create the player instance.
  Player player = {
    .spriteCount = 2,
    .spriteIndex = 0,
    .location[0] = 76,
    .location[1] = 140,
    .timeSinceAnimationStart = 0,
    .timeSinceLastMoved = 0,
    .moveSpeedDelay = 0,
    .canMove = TRUE,
    .canShoot = TRUE,
  };
  set_sprite_data(PLAYER_TILE_INDEX, player.spriteCount * 2, PlayerSprites);
  move_sprite(player.spriteIndex, player.location[0], player.location[1]);

  // Create the player bullet instance.
  Bullet playerBullet = {
  .spriteCount = 1,
  .spriteIndex = 1,
  .location[0] = 0,
  .location[1] = 0,
  .speed = 2,
  .spriteLeftOffset = 3,
  .spriteRightOffset = 3,
  .spriteTopOffset = 0,
  .spriteBottomOffset = 8,
  };
  set_sprite_data(PLAYER_BULLET_TILE_INDEX, playerBullet.spriteCount * TILE_INDEX_MULTIPLIER, BulletSprites);
  set_sprite_tile(playerBullet.spriteIndex, PLAYER_BULLET_TILE_INDEX);

  // Create an instance of enemy.
  Enemy enemy = {
    .spriteIndex = 2,
    .location[0] = 30,
    .location[1] = 30,
    .spriteLeftOffset = 0,
    .spriteRightOffset = 0,
    .spriteTopOffset = 0,
    .spriteBottomOffset = 8,
    .topEnemy = TRUE,
    .bottomEnemy = FALSE,
  };
  set_sprite_data(ENEMY_TILE_INDEX, 5 * TILE_INDEX_MULTIPLIER, MultipleEnemySprites);
  set_sprite_tile(enemy.spriteIndex, ENEMY_TILE_INDEX);
  move_sprite(enemy.spriteIndex, enemy.location[0], enemy.location[1]);

  // Game loop.
  while (1) {

    // Player controls
    if (player.canMove) {
      if (joypad()&J_LEFT) {
        moveSprite(player.spriteIndex, player.location, -1, 0);
        player.canMove = FALSE;
      }
      if (joypad()&J_RIGHT) {
        moveSprite(player.spriteIndex, player.location, 1, 0);
        player.canMove = FALSE;
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
    updatePlayer(&player);
    updatePlayerBullet(&player, &playerBullet, &enemy);
    // Animate the player.
    animateSprite(player.spriteIndex, player.spriteCount, 7, &player.timeSinceAnimationStart);
    drawEnemy(&enemy);
    performantdelay(1);
  }
  
}