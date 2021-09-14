// Based on the music player code from 0x7f (https://gbdev.gg8.se/forums/profile.php?id=936)

#include "music.h"
#include <gb/gb.h>
#include <stdbool.h>
#include "sound.h"
#include "../globals.h"

enum notes
{
	C3, Db3, D3, Eb3, E3, F3, Gb3, G3, Ab3, A3, Bb3, B3,
	C4, Db4, D4, Eb4, E4, F4, Gb4, G4, Ab4, A4, Bb4, B4,
	C5, Db5, D5, Eb5, E5, F5, Gb5, G5, Ab5, A5, Bb5, B5,
	C6, Db6, D6, Eb6, E6, F6, Gb6, G6, Ab6, A6, Bb6, B6,
	C7, Db7, D7, Eb7, E7, F7, Gb7, G7, Ab7, A7, Bb7, B7,
	C8, Db8, D8, Eb8, E8, F8, Gb8, G8, Ab8, A8, Bb8, B8,
	BRK
};

const uint16_t frequency[] =
{ 
	44, 156, 262, 363, 457, 547, 631, 710, 786, 854, 923, 986,
	1046, 1102, 1155, 1205, 1253, 1297, 1339, 1379, 1417, 1452, 1486, 1517,
	1546, 1575, 1602, 1627, 1650, 1673, 1694, 1714, 1732, 1750, 1767, 1783,
	1798, 1812, 1825, 1837, 1849, 1860, 1871, 1881, 1890, 1899, 1907, 1915,
	1923, 1930, 1936, 1943, 1949, 1954, 1959, 1964, 1969, 1974, 1978, 1982,
	1985, 1988, 1992, 1995, 1998, 2001, 2004, 2006, 2009, 2011, 2013, 2015,
	0
};

const unsigned char music_intro_lead[] =
{
  36,C5,4,BRK,36,Db5,4,BRK,36,F5,4,BRK,36,D5,4,BRK,36,F5,4,BRK,36,B4,4,BRK,36,Gb5,4,BRK,36,Ab5,4,BRK,
  25,C5,3,BRK,25,Db5,3,BRK,25,F5,3,BRK,25,D5,3,BRK,25,F5,3,BRK,25,B4,3,BRK,25,Gb5,3,BRK,25,Ab5,3,BRK,
	18,C5,2,BRK,18,Db5,2,BRK,18,F5,2,BRK,18,D5,2,BRK,18,F5,2,BRK,18,B4,2,BRK,18,Gb5,2,BRK,18,Ab5,2,BRK,
  // 9,C5,1,BRK,9,Db5,1,BRK,9,F5,1,BRK,9,D5,1,BRK,9,F5,1,BRK,9,B4,1,BRK,9,Gb5,1,BRK,9,Ab5,1,BRK,
	// 18,D5,2,BRK,18,G4,2,BRK,18,D5,2,BRK,18,G4,2,BRK,18,E5,2,BRK,18,G4,2,BRK,18,D5,2,BRK,18,G4,2,BRK,
	// 18,C5,2,BRK,18,F4,2,BRK,18,C5,2,BRK,18,F4,2,BRK,18,D5,2,BRK,18,F4,2,BRK,18,C5,2,BRK,18,F4,2,BRK,
	// 18,B4,2,BRK,18,E4,2,BRK,18,B4,2,BRK,18,E4,2,BRK,18,C5,2,BRK,18,E4,2,BRK,18,D5,2,BRK,18,E4,2,BRK,
	0
};

const unsigned char music_intro_bass[] =
{
  // 76,A3,4,BRK,76,G3,4,BRK,76,F3,4,BRK,76,E3,4,BRK,
	38,A3,2,BRK,38,G3,2,BRK,38,F3,2,BRK,38,E3,2,BRK,
  38,A3,2,BRK,38,G3,2,BRK,38,F3,2,BRK,38,E3,2,BRK,
  26,A3,2,BRK,26,G3,2,BRK,26,F3,2,BRK,26,E3,2,BRK,
  26,A3,2,BRK,26,G3,2,BRK,26,F3,2,BRK,26,E3,2,BRK,
  19,A3,1,BRK,19,G3,1,BRK,19,F3,1,BRK,19,E3,1,BRK,
  19,A3,1,BRK,19,G3,1,BRK,19,F3,1,BRK,19,E3,1,BRK,
	0
};

const unsigned char music_intro_rythm[] =
{
  2,0x4D,2,0x4F,76,0x00,2,0x4B,2,0x58,76,0x00,2,0x4B,2,0x58,76,0x00,2,0x4B,2,0x58,76,0x00,
  2,0x4D,2,0x4F,52,0x00,2,0x4B,2,0x58,52,0x00,2,0x4B,2,0x58,52,0x00,2,0x4B,2,0x58,52,0x00,
	1,0x4D,1,0x4F,38,0x00,1,0x4B,1,0x58,38,0x00,1,0x4B,1,0x58,38,0x00,1,0x4B,1,0x58,38,0x00,
  // 1,0x4D,19,0x00,1,0x4B,19,0x00,1,0x4B,19,0x00,1,0x4B,19,0x00,
	0
};

const unsigned char music_stage1_lead[] =
{
  ENEMY_MOVEMENT_DELAY-12,Db5,13,BRK,
	0
};

const unsigned char music_stage1_bass[] =
{
  // 76,A3,4,BRK,76,G3,4,BRK,76,F3,4,BRK,76,E3,4,BRK,
	ENEMY_MOVEMENT_DELAY-10,G3,11,BRK,
	0
};

const unsigned char music_stage1_rythm[] =
{
  2,0x4D,2,0x4F,ENEMY_MOVEMENT_DELAY-3,0x00,
  // 1,0x4D,19,0x00,1,0x4B,19,0x00,1,0x4B,19,0x00,1,0x4B,19,0x00,
	0
};

//wave channel
const unsigned char inst_ch3[] = {0x34,0x45,0x56,0x67,0x89,0x9A,0xAB,0xBC,0x34,0x45,0x56,0x67,0x89,0x9A,0xAB,0xBC};

#ifndef __INTELLISENSE__ //
volatile unsigned char __at (0xFF30) wave[16];
#endif


uint8_t music_play;

const unsigned char *music_data_ch1;
const unsigned char *music_ptr_ch1;
uint8_t music_cnt_ch1;
uint8_t inst_ch1;
uint8_t vol_ch1;

const unsigned char *music_data_ch2;
const unsigned char *music_ptr_ch2;
uint8_t music_cnt_ch2;
uint8_t inst_ch2;
uint8_t vol_ch2;

const unsigned char *music_data_ch3;
const unsigned char *music_ptr_ch3;
uint8_t music_cnt_ch3;

const unsigned char *music_data_ch4;
const unsigned char *music_ptr_ch4;
uint8_t music_cnt_ch4;

void init_sound() {	
	NR52_REG = 0x80;
  NR50_REG = 0x77;
  NR51_REG = 0xFF;
	
	for (uint8_t i = 0; i != 16; i++) {
    #ifndef __INTELLISENSE__ //
		wave[i] = inst_ch3[i];
    #endif
	}
}

void set_music(uint8_t song) {
	music_play = true;
		
	switch (song) {
		case MUSIC_INTRO:
			music_data_ch1 = music_intro_lead;
			music_ptr_ch1 = music_data_ch1;
			music_cnt_ch1 = 0;
			inst_ch1 = 0x80;
			vol_ch1 = 0x92;
						
			music_data_ch2 =music_intro_lead;
			music_ptr_ch2 = music_data_ch2;
			music_cnt_ch2 = 0;
			inst_ch2 = 0x00;
			vol_ch2 = 0x70;

			music_data_ch3 = music_intro_bass;
			music_ptr_ch3 = music_data_ch3;
			music_cnt_ch3 = 0;
			
			music_data_ch4 = music_intro_rythm;
			music_ptr_ch4 = music_data_ch4;
			music_cnt_ch4 = 0;
			break;
    case MUSIC_STAGE1:
			music_data_ch1 = music_stage1_lead;
			music_ptr_ch1 = music_data_ch1;
			music_cnt_ch1 = 0;
			inst_ch1 = 0x80;
			vol_ch1 = 0x92;
						
			music_data_ch2 =music_stage1_lead;
			music_ptr_ch2 = music_data_ch2;
			music_cnt_ch2 = 0;
			inst_ch2 = 0x00;
			vol_ch2 = 0x70;

			music_data_ch3 = music_stage1_bass;
			music_ptr_ch3 = music_data_ch3;
			music_cnt_ch3 = 0;
			
			music_data_ch4 = music_stage1_rythm;
			music_ptr_ch4 = music_data_ch4;
			music_cnt_ch4 = 0;
			break;
	}
}

void play_music() {
	uint16_t freq;
	uint8_t cwd;
	
	if (music_play == true) {
		// Channel 1
		if (music_cnt_ch1 == 0)	{	
			music_cnt_ch1 = *music_ptr_ch1;
			music_ptr_ch1++;
			freq = frequency[*music_ptr_ch1];
			music_ptr_ch1++;
			
			if (sound_cnt_ch1 == 0) {
				if (freq == 0) {
					NR10_REG = 0;
					NR11_REG = 0;
					NR12_REG = 0;
					NR13_REG = 0;
					NR14_REG = 0;	
				} else {
					NR10_REG = 0;
					NR11_REG = inst_ch1;
					NR12_REG = vol_ch1;
					NR13_REG = freq & 0x00FF;
					NR14_REG = 0x80 | ((freq >> 8) & 0x0007);	
				}
			}
						
			if (*music_ptr_ch1 == 0) {
				music_ptr_ch1 = music_data_ch1;
			}
		} 
		music_cnt_ch1--;
		
		// Channel 2
		if (music_cnt_ch2 == 0) {	
			music_cnt_ch2 = *music_ptr_ch2;
			music_ptr_ch2++;
			freq = frequency[*music_ptr_ch2];
			music_ptr_ch2++;
      
			if (freq == 0) {
				NR21_REG = 0;
				NR22_REG = 0;
				NR23_REG = 0;
				NR24_REG = 0;		
			} else {
				NR21_REG = inst_ch2;
				NR22_REG = vol_ch2;
				NR23_REG = freq & 0x00FF;
				NR24_REG = 0x80 | ((freq >> 8) & 0x0007);		
			}
			
			if (*music_ptr_ch2 == 0) {
				music_ptr_ch2 = music_data_ch2;
			}
		} 
		music_cnt_ch2--;
		
		// Channel 3
		if (music_cnt_ch3 == 0) {	
			music_cnt_ch3 = *music_ptr_ch3;
			music_ptr_ch3++;
			freq = frequency[*music_ptr_ch3];
			music_ptr_ch3++;

			if (freq == 0) {
				NR30_REG = 0;
				NR31_REG = 0;
				NR32_REG = 0;
				NR33_REG = 0;
				NR34_REG = 0;
			} else {
				NR30_REG = 0x80;
				NR31_REG = 0xFF;
				NR32_REG = 0x20;
				NR33_REG = freq & 0x00FF;
				NR34_REG = 0x80 | ((freq >> 8) & 0x0007);	
			}
				
			if (*music_ptr_ch3 == 0) {
				music_ptr_ch3 = music_data_ch3;
			}	
		} 
		music_cnt_ch3--;	
			
		// Channel 4
		if (music_cnt_ch4 == 0) {	
			music_cnt_ch4 = *music_ptr_ch4;
			music_ptr_ch4++;
			cwd = *music_ptr_ch4;
			music_ptr_ch4++;
      
			if (sound_cnt_ch4 == 0) {
				if (cwd == 0) {
					NR41_REG = 0;
					NR42_REG = 0;
					NR43_REG = 0;
					NR44_REG = 0;		
				} else {
					NR41_REG = 0xFF;
					NR42_REG = 0x90;
					NR43_REG = cwd;
					NR44_REG = 0x80;
				}
			}
			if (*music_ptr_ch4 == 0) {
				music_ptr_ch4 = music_data_ch4;
			}	
		} 
		music_cnt_ch4--;	
	}
}

void stop_music() {
	NR12_REG = 0;
	NR14_REG = 0;
	
	NR22_REG = 0;
	NR24_REG = 0;
	
	NR32_REG = 0;
	NR34_REG = 0;
	
	NR42_REG = 0;
	NR44_REG = 0;
		
	music_play = false;
}

void restart_music() {
	music_play = true;
	
	music_cnt_ch1 = 0;
	music_cnt_ch2 = 0;
	music_cnt_ch3 = 0;
	music_cnt_ch4 = 0;	
	
	music_ptr_ch1 = music_data_ch1;
	music_ptr_ch2 = music_data_ch2;
	music_ptr_ch3 = music_data_ch3;
	music_ptr_ch4 = music_data_ch4;
}
