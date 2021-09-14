// Based on the music player code from 0x7f (https://gbdev.gg8.se/forums/profile.php?id=936)

#include "sound.h"
#include <gb/gb.h>

const unsigned char soundPlayerShoot[][6] =
{
  {0x01,0x15,0x82,0xD4,0x23,0xDF}
};

const unsigned char soundExplosion[][5] =
{
	{0x01,0x3F,0xF1,0x54,0x80},
	{0x01,0x3F,0xF2,0x45,0x80},
	{0x01,0x3F,0xF2,0x54,0x80},
	{0x01,0x3F,0xF2,0x45,0x80},
	{0x01,0x3F,0xF2,0x35,0x80},
	{0x01,0x3F,0xF2,0x35,0x80},
  {0x01,0x3F,0xF2,0x04,0x80}
};

const unsigned char soundEnemiesMove[][6] = 
{
  {0x01,0x33,0x80,0xF4,0xDF,0x84}
};

uint8_t sound_cnt_ch1;
uint8_t sound_typ_ch1;

uint8_t sound_cnt_ch4;
uint8_t sound_typ_ch4;

void set_sound(uint8_t sound) {
	switch (sound) {
		case SOUND_PLAYER_SHOOT:
      sound_cnt_ch1 = (sizeof(soundPlayerShoot)) / 6;
      sound_typ_ch1 = SOUND_PLAYER_SHOOT;
			break;
    case SOUND_EXPLOSION:
      sound_cnt_ch4 = (sizeof(soundExplosion)) / 5;
      sound_typ_ch4 = SOUND_EXPLOSION;
      break;
    case SOUND_ENEMIES_MOVE:
      sound_cnt_ch1 = (sizeof(soundEnemiesMove)) / 6;
      sound_typ_ch1 = SOUND_ENEMIES_MOVE;
      break;
	}
}

void play_sound() {
	if (sound_cnt_ch4 != 0)	{
		sound_cnt_ch4--;
		switch (sound_typ_ch4) {
			case SOUND_EXPLOSION:
				if (soundExplosion[sound_cnt_ch4][0] == 1) {
					NR41_REG = soundExplosion[sound_cnt_ch4][1];
					NR42_REG = soundExplosion[sound_cnt_ch4][2];
					NR43_REG = soundExplosion[sound_cnt_ch4][3];
					NR44_REG = soundExplosion[sound_cnt_ch4][4];
				}
				break;
		}
	}
	if (sound_cnt_ch1 != 0) {
		sound_cnt_ch1--;
		switch (sound_typ_ch1) {
      case SOUND_PLAYER_SHOOT:
				if (soundPlayerShoot[sound_cnt_ch1][0] == 1) {
					NR10_REG = soundPlayerShoot[sound_cnt_ch1][1];
					NR11_REG = soundPlayerShoot[sound_cnt_ch1][2];
					NR12_REG = soundPlayerShoot[sound_cnt_ch1][3];
					NR13_REG = soundPlayerShoot[sound_cnt_ch1][4];
					NR14_REG = soundPlayerShoot[sound_cnt_ch1][5];
				}
        break;
      case SOUND_ENEMIES_MOVE:
				if (soundEnemiesMove[sound_cnt_ch1][0] == 1) {
					NR10_REG = soundEnemiesMove[sound_cnt_ch1][1];
					NR11_REG = soundEnemiesMove[sound_cnt_ch1][2];
					NR12_REG = soundEnemiesMove[sound_cnt_ch1][3];
					NR13_REG = soundEnemiesMove[sound_cnt_ch1][4];
					NR14_REG = soundEnemiesMove[sound_cnt_ch1][5];
				}
        break;
		}
	}
}