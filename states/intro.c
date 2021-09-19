#include <gb\gb.h>
#include "..\globals.h"
#include "..\sound\music.h"
#include "..\maps\intro_map.h"
#include "..\states\game.h"

void init_intro() {
  HIDE_WIN;
  HIDE_SPRITES;
  SHOW_BKG;

  set_bkg_tiles(0, 0, 20, 18, intro_map);

  set_music(MUSIC_INTRO);
}

void run_intro() {
  UPDATE_KEYS();

  if (KEY_TICKED(J_START)) {
    init_game();
    game_state = STATE_GAME;
  }

  // If music is set, play it.
  play_music();

  wait_vbl_done();
}