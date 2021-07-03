#include <gb/gb.h>

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
