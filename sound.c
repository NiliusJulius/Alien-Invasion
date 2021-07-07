// Based on the music player code from 0x7f (https://gbdev.gg8.se/forums/profile.php?id=936)

#include <gb/gb.h>

#define SOUND_PLAYER_SHOOT 0
#define SOUND_EXPLOSION 1

const unsigned char soundPlayerShoot[][6] =
{
  {0x01,0x33,0x80,0xF4,0xDF,0x84}
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
	}
}

void play_sound() {
	if (sound_cnt_ch4 != 0)	{
		sound_cnt_ch4--;
		switch (sound_typ_ch4) {
			case 1:
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
      default:
				if (soundPlayerShoot[sound_cnt_ch1][0] == 1) {
					NR10_REG = soundPlayerShoot[sound_cnt_ch1][1];
					NR11_REG = soundPlayerShoot[sound_cnt_ch1][2];
					NR12_REG = soundPlayerShoot[sound_cnt_ch1][3];
					NR13_REG = soundPlayerShoot[sound_cnt_ch1][4];
					NR14_REG = soundPlayerShoot[sound_cnt_ch1][5];
				}
        break;
		}
	}
}