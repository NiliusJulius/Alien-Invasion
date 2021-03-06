#ifndef _SOUND
#define _SOUND

#include <stdint.h>

#define SOUND_PLAYER_SHOOT 0
#define SOUND_EXPLOSION 1
#define SOUND_ENEMIES_MOVE 2

extern uint8_t sound_cnt_ch1;
extern uint8_t sound_typ_ch1;

extern uint8_t sound_cnt_ch4;
extern uint8_t sound_typ_ch4;

void set_sound(uint8_t sound);
void play_sound();

#endif