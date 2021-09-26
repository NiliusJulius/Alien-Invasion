#include <gb\gb.h>
#include "..\globals.h"
#include "..\sound\music.h"
#include "..\maps\game_over_map.h"
#include "..\states\game.h"
#include "..\states\intro.h"

void init_game_over() {
  HIDE_WIN;
  HIDE_SPRITES;
  SHOW_BKG;

  if (score > high_score) {
    high_score = score;
  }

  set_bkg_tiles(0, 0, 20, 18, game_over_map);

  unsigned char score_buffer[9];
  bcd2text(&score, 0x01, score_buffer);
  set_bkg_tiles(5, 8, 8, 1, score_buffer);

  unsigned char high_score_buffer[9];
  bcd2text(&high_score, 0x01, high_score_buffer);
  set_bkg_tiles(5, 13, 8, 1, high_score_buffer);

  set_music(MUSIC_GAME_OVER);
}

void run_game_over() {
  UPDATE_KEYS();

  if (KEY_TICKED(J_START)) {
    init_intro();
    game_state = STATE_INTRO;
  }

  // If music is set, play it.
  play_music();

  wait_vbl_done();
}