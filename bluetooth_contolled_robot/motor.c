#include "motor.h"
#include "led.h"

void init_motor_control_lines(void) {
	// Enable clock to ports B and D
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTE_MASK;


	// Make 3 pins GPIO
	PORTB->PCR[MOTOR1_CONTROL1A] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[MOTOR1_CONTROL1A] |= PORT_PCR_MUX(1);
	PORTB->PCR[MOTOR1_CONTROL2A] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[MOTOR1_CONTROL2A] |= PORT_PCR_MUX(1);
	PORTC->PCR[MOTOR2_CONTROL3A] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[MOTOR2_CONTROL3A] |= PORT_PCR_MUX(1);
	PORTC->PCR[MOTOR2_CONTROL4A] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[MOTOR2_CONTROL4A] |= PORT_PCR_MUX(1);
	PORTB->PCR[RED_LED_POS] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED_POS] |= PORT_PCR_MUX(1);
	

	//monitoring only
	//PORTB->PCR[RED] &= ~PORT_PCR_MUX_MASK;
	//PORTB->PCR[RED] |= PORT_PCR_MUX(3);

	// Set ports to outputs
	PTB->PDDR |= MASK(MOTOR1_CONTROL1A) | MASK(MOTOR1_CONTROL2A) | MASK(RED_LED_POS); //| MASK(RED) ;
	PTB->PSOR |=MASK(MOTOR1_CONTROL1A);		//on initially
	PTB->PCOR |=MASK(MOTOR1_CONTROL2A);		//off initially
	PTB->PSOR |=MASK(18);
	
	PTC->PDDR	|= MASK(MOTOR2_CONTROL3A) | MASK(MOTOR2_CONTROL4A) ;
	PTC->PCOR |=MASK(MOTOR2_CONTROL3A);		//off initially
	PTC->PSOR |=MASK(MOTOR2_CONTROL4A);		//on initially

	//configure TPM/pwm  output
	PORTB->PCR[MOTOR1_ENABLE ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTB->PCR[MOTOR1_ENABLE] |= PORT_PCR_MUX(3);	//***setup to be output of TPM2_CH0****
	PORTE->PCR[MOTOR2_ENABLE ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTE->PCR[MOTOR2_ENABLE] |= PORT_PCR_MUX(3);	//***setup to be output of TPM2_CH1****
}



void init_motor_timer(){
	//Clock gate
	SIM->SCGC6 |=SIM_SCGC6_TPM2_MASK;	//*******TPM2 channel 0
	//Select clock source in SIM_SOPT
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);	//1- MCGPLLCLK/2, or MCGFLLCLK 01 (ext?), 2-ext OSCERCLK, 3-internal MCGIRCLK
	//Configure registers
	TPM2->MOD= 174763;  //10Hz period -> 20971520/(10*prescale) = 174762.667

	//working wth TPM2_C0SC
						//output compare + edge aligned PWM MSBA: 10, ELSBA:10
	TPM2->CONTROLS[0].CnSC |= TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1) ;
	TPM2->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK;  //clear spurious interrupts
	TPM2->CONTROLS[0].CnV =174762;  //full power;
	
	TPM2->CONTROLS[1].CnSC |= TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1) ;
	TPM2->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK;  //clear spurious interrupts
	TPM2->CONTROLS[1].CnV =174762;  //full-power;
	
	TPM2->SC |=  TPM_SC_TOF_MASK | TPM_SC_PS(3) | TPM_SC_TOIE_MASK  ;

	TPM2->SC |= TPM_SC_CMOD(1); //  internal clock to run

	NVIC_ClearPendingIRQ(TPM2_IRQn);
	NVIC_SetPriority(TPM2_IRQn, 3);
	NVIC_EnableIRQ(TPM2_IRQn);
}



void TPM2_IRQHandler(){

//	static int ctr=0;
//	static int fwd=0;		//to change direction
//	static enum stages{ stage1,stage2,stage3}next_stage=stage1;
//	if (TPM2->SC & TPM_SC_TOF_MASK){	//count overflow
//		ctr++;
//		if (ctr>=250){  //wait for 5secs
//			ctr=0;
//			if (!ctr)
//				fwd++;		//count only when stage changes
//			switch (next_stage){
//			case stage1:
//				fwd++;
//				TPM2->CONTROLS[0].CnV =87382; //half power
//				next_stage=stage2;
//				break;
//			case stage2:
//				TPM2->CONTROLS[0].CnV =131072; //3/4 power
//				next_stage=stage3;
//				break;

//			case stage3:
//				TPM2->CONTROLS[0].CnV =174763; // full power 
//				next_stage=stage1;
//				break;
//			default:
//				TPM2->CONTROLS[0].CnV =174763; 
//				next_stage=stage1;
//				break;
//			}
//		}
//		}
		
	TPM2->SC |= TPM_SC_TOF_MASK ; //clear the interrupt
}


void reverse_motors(){
	
	unsigned long current_time;
	static unsigned long last_run=0;
	
	current_time=millis();
	PTD -> PSOR |= MASK(7);
	if ((current_time-last_run)>=500){
		PTD -> PCOR |= MASK(7);
	}
	PTD -> PSOR |= MASK(7);

	
	
	PTC->PSOR |= MASK(TAIL_RED_LED);
	PTC->PCOR |= MASK(TAIL_WHITE_LED);
	
	PTB->PCOR |=MASK(MOTOR1_CONTROL1A);		//turn off 1A
	PTB->PSOR |=MASK(MOTOR1_CONTROL2A);		//turn on 2A
	
	PTC->PSOR |=MASK(MOTOR2_CONTROL3A);		//turn off 3A
	PTC->PCOR |=MASK(MOTOR2_CONTROL4A);		//turn on 4A
	
}


void forward_motors(){
	PTC->PCOR |= MASK(TAIL_RED_LED);
	PTC->PSOR |= MASK(TAIL_WHITE_LED);
	PTB->PSOR |=MASK(MOTOR1_CONTROL1A);		//turn on 1A
	PTB->PCOR |=MASK(MOTOR1_CONTROL2A);		//turn off 2A
	
	PTC->PCOR |=MASK(MOTOR2_CONTROL3A);		//turn off 3A
	PTC->PSOR |=MASK(MOTOR2_CONTROL4A);		//turn on 4A
}

void stop_car(){
	PTB->PCOR |=MASK(MOTOR1_CONTROL1A);		//turn off 1A
	PTB->PCOR |=MASK(MOTOR1_CONTROL2A);		//turn off 2A
	
	PTC->PCOR |=MASK(MOTOR2_CONTROL3A);		//turn off 3A
	PTC->PCOR |=MASK(MOTOR2_CONTROL4A);		//turn off 4A
}

void turn_car_left(){
	//	Motor 2 is on the left
	//  Motor 1 is on the right
	//	turn left by slowing down motor 2 and increasing speed of motor 1
	
		TPM2->CONTROLS[0].CnV =174763; //full power

		//TPM2->CONTROLS[1].CnV =87382; // half power 
		PTC->PCOR |=MASK(MOTOR2_CONTROL3A);		//turn off 3A
		PTC->PCOR |=MASK(MOTOR2_CONTROL4A);		//turn on 4A
}


void turn_car_right(){
	//	Motor 2 is on the left
	//  Motor 1 is on the right
	//	turn left by slowing down motor 1 and increasing speed of motor 2
	
		TPM2->CONTROLS[1].CnV =174763; //half power
		//TPM2->CONTROLS[1].CnV =87382; // full power 
		PTB->PCOR |=MASK(MOTOR1_CONTROL1A);		//turn off 1A
		PTB->PCOR |=MASK(MOTOR1_CONTROL2A);		//turn off 2A
}


void increase_speed_progressively(){	
	static enum stages{stage1,stage2,stage3}next_stage=stage1;
	unsigned long current_time;
	static unsigned long last_run=0;

	switch (next_stage){
			case stage1:
				forward_motors();
				TPM2->CONTROLS[0].CnV =87382; //half power
				TPM2->CONTROLS[1].CnV =87382; //half power
			if ((current_time-last_run)>=500){
				last_run=current_time;
				next_stage=stage2;
			}
				break;
			case stage2:
				TPM2->CONTROLS[0].CnV =131072; //3/4 power
				TPM2->CONTROLS[1].CnV =131072; //3/4 power
				if ((current_time-last_run)>=500){
				last_run=current_time;
				next_stage=stage3;
				}
				break;

			case stage3:
				TPM2->CONTROLS[0].CnV =174763; // full power 
				TPM2->CONTROLS[1].CnV =174763; // full power 
				if ((current_time-last_run)>=500){
				last_run=current_time;
				next_stage=stage1;
				}
				break;
			default:
				next_stage=stage3;
				break;
	}
}

		
void slow_down(){
	TPM2->CONTROLS[0].CnV =87382; //half power
	TPM2->CONTROLS[1].CnV =87382; // half power 
}

void move_faster(){
	TPM2->CONTROLS[0].CnV =174763; //full power
	TPM2->CONTROLS[1].CnV =174763; // full power 
}
