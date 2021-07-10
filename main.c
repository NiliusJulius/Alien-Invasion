#include <gb/font.h>

#include "globals.h"

#include "sound\music.h"
#include "sound\sound.h"

#include "states\level.h"

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

  init_game();
  game_state = STATE_LEVEL;
  
  while (1) {
    switch (game_state) {
      case STATE_LEVEL:
        run_level();
        break;
    }
    
  }
  
}