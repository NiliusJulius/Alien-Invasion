#include <gb\gb.h>
#include "..\globals.h"
#include "..\sound\music.h"
#include "..\maps\intro_map.h"
#include "..\states\game.h"

void init_intro() {
  SHOW_BKG;

  set_bkg_tiles(7, 8, 5, 2, intro_map);

  set_music(MUSIC_INTRO);
}

void run_intro() {
  UPDATE_KEYS();

  if (KEY_TICKED(J_START)) {
    init_game();
    game_state = STATE_GAME;
  }

  wait_vbl_done();
}