#ifndef SOUND_H_
#define SOUND_H_

#include "common.h"


#define PTE30_PIN	30

#define ENDING_SOUND_LEN 25
#define RUNNING_SOUND_LEN 96


void play_running_sound(void);
void play_ending_sound(void);
void stop_sound(void);
void initSound(void);

#endif
