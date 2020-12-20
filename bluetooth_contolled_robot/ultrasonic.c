#include "general.h"
#include "ultrasonic.h"

unsigned int interval=0; //interval for input capture-time between rising edge
unsigned char distance = 0;


void init_ultrasonic_pins(){

	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK ;
	PORTB -> PCR[ECHO_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[ECHO_PIN] |= PORT_PCR_MUX(3);  //***setup to be output of TPM1_CH0****
	
	
	PORTD->PCR[TRIGGER_PIN] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[TRIGGER_PIN] |= PORT_PCR_MUX(4);	//***setup to be input of TPM0_CH0***
	
	PORTD->PCR[BUZZER] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BUZZER] |= PORT_PCR_MUX(1);
	PTD->PDDR	|= MASK(BUZZER) | MASK(BUZZER) ;
	
}


void init_ultrasonic_timer(){
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK | SIM_SCGC6_TPM0_MASK;  //TPM1 AND TPM0 
	// Select clock source in SIM_SOPT
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	TPM0->MOD = 12780u;  //12779.8.... 78 milliseconds
	
	//Setting up for input capture- MSA-MSB(00) ELSB-ELSA(11)
	TPM1->CONTROLS[0].CnSC |= TPM_CnSC_ELSB(1) | TPM_CnSC_ELSA(1); //rising and falling edge
	TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK | TPM_CnSC_CHIE_MASK; //enable interrupts

	
	//setting up for edge aligned PWM on trig pin: MSB-MSA: 10, ELSB-ELSA:10 - true high
	TPM0->CONTROLS[0].CnSC |= TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1) ;
	TPM0->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK |  TPM_CnSC_CHIE_MASK;  //clear spurious interrupts and enable interrupts
	TPM0->CONTROLS[0].CnV = 3u;	//20 microseconds
	

	TPM1->SC |= TPM_SC_TOF_MASK | TPM_SC_PS(7) | TPM_SC_TOIE_MASK; //Prescale value = 2^7 = 128
	TPM0->SC |= TPM_SC_TOF_MASK | TPM_SC_PS(7) | TPM_SC_TOIE_MASK; //Prescale value = 2^7 = 128
	
	TPM1->SC |= TPM_SC_CMOD(1); //enable internal clock to run
	TPM0->SC |= TPM_SC_CMOD(1); //enable internal clock to run
	
	NVIC_ClearPendingIRQ(TPM1_IRQn);
	NVIC_SetPriority(TPM1_IRQn, 3);
	NVIC_EnableIRQ(TPM1_IRQn);
	
	__enable_irq();
}





void TPM1_IRQHandler(){
		static int ctr=0;
		static unsigned int previous=0;
		unsigned int current=0;
		
		if (TPM1->STATUS & TPM_STATUS_CH0F_MASK){// check if input(rising or falling) occurred
				current=TPM1->CONTROLS[0].CnV;
				
				current |= (ctr <<16); // add the no. of overflows. current = CnV + ctr*MOD [(ctr*2^16)]
				//Each ctr tick is 2^16,
				//without above, current value could also be more than prev.
				interval = current-previous;
				previous=current;
			
				//sprintf(buffer, "Distance: %d", interval);
				//lcd_goto(0,1);
				//lcd_write_string(buffer);
				TPM1->CONTROLS[0].CnSC |=TPM_CnSC_CHF_MASK; //clear input capture flag
		}

		if (TPM1->SC & TPM_SC_TOF_MASK){
				ctr++; //a timer overflow occurred.
				TPM1->SC |= TPM_SC_TOF_MASK ; //clear the interrupt on timer overflow
		}
}

void raise_obstacle_alarm(){
	if (distance <= 20){
	static enum buzzer_states {S1, S2, S3, S4} next_state=S1;
	unsigned long current_time;
	static unsigned long last_run=0;
	
	current_time=millis();
	switch (next_state){
		case S1:
			PTD -> PSOR |= MASK(BUZZER);
			if ((current_time-last_run)>=50){
				last_run=current_time;
				next_state=S2;
			}
			break;
		case S2:
			PTD -> PCOR |= MASK(BUZZER);
			if ((current_time-last_run)>=50){
				last_run=current_time;
				next_state=S3;
			}
			break;
		case S3:
			PTD -> PSOR |= MASK(BUZZER);
			if ((current_time-last_run)>=50){
				last_run=current_time;
				next_state=S4;
			}
			break;
			
		default:
			PTD -> PCOR |= MASK(BUZZER);
			if ((current_time-last_run)>=50){
				last_run=current_time;
				next_state=S2;
			}
			break;
	}
}
	else{
		PTD -> PCOR |= MASK(BUZZER);
	}
}
