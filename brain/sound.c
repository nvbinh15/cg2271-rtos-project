#include "MKL25Z4.h"
#include "sound.h"
#include "cmsis_os2.h"

int ending_song[] = { // happy birthday
	NOTE_C4, NOTE_C4, 
  NOTE_D4, NOTE_C4, NOTE_F4,
  NOTE_E4, NOTE_C4, NOTE_C4,
  NOTE_D4, NOTE_C4, NOTE_G4,
  NOTE_F4, NOTE_C4, NOTE_C4,

  NOTE_C5, NOTE_A4, NOTE_F4, 
  NOTE_E4, NOTE_D4, NOTE_AS4, NOTE_AS4,
  NOTE_A4, NOTE_F4, NOTE_G4,
  NOTE_F4
};

int running_song[] = {
	NOTE_FS5, NOTE_FS5,NOTE_D5, NOTE_B4, REST, NOTE_B4, REST, NOTE_E5, 
  REST, NOTE_E5, REST, NOTE_E5, NOTE_GS5, NOTE_GS5, NOTE_A5, NOTE_B5,
  NOTE_A5, NOTE_A5, NOTE_A5, NOTE_E5, REST, NOTE_D5, REST, NOTE_FS5, 
  REST, NOTE_FS5, REST, NOTE_FS5, NOTE_E5, NOTE_E5, NOTE_FS5, NOTE_E5,
  NOTE_FS5, NOTE_FS5,NOTE_D5, NOTE_B4, REST, NOTE_B4, REST, NOTE_E5, 
  
  REST, NOTE_E5, REST, NOTE_E5, NOTE_GS5, NOTE_GS5, NOTE_A5, NOTE_B5,
  NOTE_A5, NOTE_A5, NOTE_A5, NOTE_E5, REST, NOTE_D5, REST, NOTE_FS5, 
  REST, NOTE_FS5, REST, NOTE_FS5, NOTE_E5, NOTE_E5, NOTE_FS5, NOTE_E5,
  NOTE_FS5, NOTE_FS5,NOTE_D5, NOTE_B4, REST, NOTE_B4, REST, NOTE_E5, 
  REST, NOTE_E5, REST, NOTE_E5, NOTE_GS5, NOTE_GS5, NOTE_A5, NOTE_B5,
  
  NOTE_A5, NOTE_A5, NOTE_A5, NOTE_E5, REST, NOTE_D5, REST, NOTE_FS5, 
  REST, NOTE_FS5, REST, NOTE_FS5, NOTE_E5, NOTE_E5, NOTE_FS5, NOTE_E5
};


/* Initialize PWM module */
void initSound(void) {
	// Enable Clock Gating for PORTB
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK; // SIM_SCGC5 != (1ul << 10);
	
	// Configure Mode 3 for the PWM pin operation (alternate 3 -> PWM)
	PORTA->PCR[PTA13_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[PTA13_Pin] |= PORT_PCR_MUX(3);	
	
	// Enable Clock Gating for Timer1
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
	
	// Select internal clock for TPM module
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;	
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);	// SIM->SOPT2[25:24] = 01
	
	
	// Edge-Aligned PWM
	// Update SnC register: CMOD = 01, PS = 111 (128)
	TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));	// clear previous data
	TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));			// CMOD = 01 -> select internal clock, PS = 111 -> prescaler = 128
	TPM1->SC &= ~(TPM_SC_CPWMS_MASK);						// CPWMS = 0 -> count up
	
	// Enable PWM on TPM1 Channel 1 -> PTB1, B bits are 1, A bits are 0 for edge-align
	TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
}


void play_running_sound(void) {
	
	//for (;;) {
		for (int i = 0; i < RUNNING_SOUNT_LEN; i++) {
			if (running_song[i] != 0) {
				TPM1->MOD = 375000 / running_song[i];
				TPM1_C1V =  375000 / running_song[i] / 10;
			}
			osDelay(200);
		}
	//}
}

void play_ending_sound(void) {
	
	//for (;;) {
	for (int i = 0; i < ENDING_SOUND_LEN; i++) {
			if (ending_song[i] != 0) {
				TPM1->MOD = 375000 / ending_song[i];
				TPM1_C1V =  375000 / ending_song[i] / 10;
			}
			osDelay(200);
		}
	//}
}

/* Stops the audio */
void stop_sound(void) {
    TPM0_MOD = 100;
		TPM1_C1V = 0;
    osDelay(50);
}



	