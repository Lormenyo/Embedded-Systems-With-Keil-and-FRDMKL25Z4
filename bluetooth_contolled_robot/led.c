#include "general.h"
#include "led.h"

extern int potentiometer_value;

void init_leds(){

	// Enable clock to ports B and D
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTE_MASK | SIM_SCGC5_PORTD_MASK;


	// Make 3 pins GPIO
	PORTC->PCR[EMERGENCY_BLUE_LED] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[EMERGENCY_BLUE_LED] |= PORT_PCR_MUX(1);
	
	PORTC->PCR[EMERGENCY_RED_LED] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[EMERGENCY_RED_LED] |= PORT_PCR_MUX(1);
	
	PORTD->PCR[POTENTIOMETER_LED] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[POTENTIOMETER_LED] |= PORT_PCR_MUX(1);
	
	PORTC->PCR[TAIL_WHITE_LED] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[TAIL_WHITE_LED] |= PORT_PCR_MUX(1);
	
	PORTC->PCR[TAIL_RED_LED] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[TAIL_RED_LED] |= PORT_PCR_MUX(1);
	
	PTC->PDDR	|= MASK(EMERGENCY_BLUE_LED) | MASK(EMERGENCY_RED_LED) | MASK(TAIL_RED_LED) | MASK(TAIL_WHITE_LED);
	PTD->PDDR	|= MASK(POTENTIOMETER_LED);
}


void flashEmergencyLights(){
	static enum red_states {S1, S2, S3, S4, S5, S6, S7} next_state=S1;
	unsigned long current_time;
	static unsigned long last_run=0;
	
	current_time=millis();
	switch (next_state){
		case S1:
			PTC->PSOR |= MASK(EMERGENCY_RED_LED);
			PTC->PCOR |= MASK(EMERGENCY_BLUE_LED);
			if ((current_time-last_run)>=200){
				last_run=current_time;
				next_state=S2;
			}
			break;
		case S2:
			PTC->PCOR |= MASK(EMERGENCY_RED_LED);
			PTC->PSOR |= MASK(EMERGENCY_BLUE_LED);
			if ((current_time-last_run)>=200){
				last_run=current_time;
				next_state=S3;
			}
			break;
			case S3:
			PTC->PCOR |= MASK(EMERGENCY_RED_LED);
			PTC->PCOR |= MASK(EMERGENCY_BLUE_LED);
			if ((current_time-last_run)>=200){
				last_run=current_time;
				next_state=S4;
			}
			break;
		case S4:
			PTC->PCOR |= MASK(EMERGENCY_RED_LED);
			PTC->PSOR |= MASK(EMERGENCY_BLUE_LED);
			if ((current_time-last_run)>=200){
				last_run=current_time;
				next_state=S5;
			}
			break;
		case S5:
			PTC->PCOR |= MASK(EMERGENCY_RED_LED);
			PTC->PCOR |= MASK(EMERGENCY_BLUE_LED);
			if ((current_time-last_run)>=200){
				last_run=current_time;
				next_state=S6;
			}
			break;
		case S6:
			PTC->PCOR |= MASK(EMERGENCY_RED_LED);
			PTC->PSOR |= MASK(EMERGENCY_BLUE_LED);
			if ((current_time-last_run)>=200){
				last_run=current_time;
				next_state=S1;
			}
			break;
		case S7:
			PTC->PCOR |= MASK(EMERGENCY_RED_LED);
			PTC->PCOR |= MASK(EMERGENCY_BLUE_LED);
			if ((current_time-last_run)>=200){
				last_run=current_time;
				next_state=S1;
			}
			break;
		default:
			next_state =S1;
			break;
	}
}




void init_ADC(void) {

	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	// Select analog for pin
	PORTE->PCR[POTENTIOMETER_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[POTENTIOMETER_PIN] |= PORT_PCR_MUX(0);

	// use low power and long sample time improves accuracy (& uses less power)
	ADC0->CFG1 = ADC_CFG1_ADLPC_MASK | ADC_CFG1_ADLSMP_MASK ;
	//16 bit single-ended conversion, when DIFF=0
	ADC0->CFG1 |= ADC_CFG1_MODE(3);
	// Default settings: Software trigger, voltage references VREFH and VREFL
	ADC0->SC2 = ADC_SC2_REFSEL(0);

}


int measure_POT(void){
	int val;
	unsigned int sample =0;

	ADC0->SC1[0] = 0x00; // start conversion on channel 0, (the software trigger & chan selection)

	while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
	sample = ADC0->R[0];
	val = (int)3.3*sample/0xffff;		//recall 0xffff maps to max of 3.3v
	return val;
}
