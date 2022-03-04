#include "MKL25Z4.H"

#define PTB0_Pin	0
#define PTB1_Pin	1

// Musical tones
#define C 262
#define D 294
#define E 330
#define F 349
#define G 392
#define A 440
#define B 494

int tones[7] = {262, 294, 330, 349, 392, 440, 494};
//int tones[7] = {50, 100, 200, 400, 800, 1600, 3200};

static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}

void initPWM(void) {
	// Enable Clock Gating for PORTB
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	// Configure Mode 3 for the PWM pin operation
	PORTB->PCR[PTB0_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB0_Pin] |= PORT_PCR_MUX(3);
	
	PORTB->PCR[PTB1_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB1_Pin] |= PORT_PCR_MUX(3);	
	
	// Enable Clock Gating for Timer1
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
	
	// Select clock for TPM module
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
	// Set modulo value 20971520 / 128 = 163840 / 3276 = 50 Hz
	// TPM1->MOD = 3276;
	
	// Set modulo value 48000000 / 128 = 375000 / 7500 = 50 Hz
	//TPM1->MOD = 7500;
	
	// Edge-Aligned PWM
	// Update SnC register: CMOD = 01, PS = 111 (128)
	TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	TPM1->SC &= ~(TPM_SC_CPWMS_MASK);
	
	// Enable PWM on TPM1 Channel 0 -> PTB0
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	// Enable PWM on TPM1 Channel 1 -> PTB1
	TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
}

int main(void) {
//	SystemCoreClockUpdate();
	initPWM();
	int count = 0;
	
	// Set TPM1_C0V
	//TPM1_C0V = 0x0EA6; // 0x0EA6 = 3750 = 7500 / 2 -> 50% duty cycle
	//TPM1_C1V = 0x0753; // 0x0753 = 1875 = 7500 / 4 -> 25% duty cycle
	
	while (1) {
		if (count == 7) {
			count = 0;
		}
		
		TPM1->MOD = 48000000 / 128 / tones[count];
		TPM1_C0V =  48000000 / 128 / tones[count] / 2;
		
		count++;
		delay(0xFFFFFF);
	}
}