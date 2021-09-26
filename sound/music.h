#ifndef _MUSIC
#define _MUSIC

#include <stdint.h>

#define MUSIC_INTRO 0
#define MUSIC_GAME_OVER 1
void init_sound();
void set_music(uint8_t song);
void play_music();
void stop_music();
void restart_music();

#endif