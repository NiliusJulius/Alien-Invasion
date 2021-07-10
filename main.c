#include <gb/font.h>

#include "globals.h"

#include "sound\music.h"
#include "sound\sound.h"

#include "states\game.h"
#include "states\intro.h"

void main() {

  // Create font.
  font_t min_font;
  font_init();
  min_font = font_load(font_min);
  font_set(min_font);

  // Initialize sound and music.
  sound_cnt_ch1 = 0;
  sound_cnt_ch4 = 0;
  init_sound();

  init_intro();
  game_state = STATE_INTRO;
  
  while (1) {
    switch (game_state) {
      case STATE_INTRO:
        run_intro();
        break;
      case STATE_GAME:
        run_game();
        break;
    }
    
  }
}