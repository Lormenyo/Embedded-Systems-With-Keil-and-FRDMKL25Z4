#include <mkl25z4.h>
#include "lcd.h"
#include <stdio.h>
#include <string.h>

#define ECHO_PIN_RISING (0) //PTB0 - TPM1_CH0 -ALT3
//#define ECHO_PIN_FALLING (1) //PTB1 - TPM1_CH1 - ALT3
#define TRIGGER_PIN (0) //PTD0 - TPM0_CH0 ALT4

int calculate_time(int interval);
int calculate_distance(int time);
void init_pins(void);
void init_timer(void);


unsigned int interval=0; //interval for input capture-time between rising edge
unsigned char distance = 0;
unsigned int time = 0;
char buffer[10];


int main(void){
	init_lcd_pins();
	init_pins();
	init_timer();

	
	lcd_init();
	lcd_clear();
	
	lcd_goto(0,0);
	lcd_write_string("Ultrasonic Sensor");
	

		while(1){
			distance = interval * 0.10468;
			sprintf(buffer, "Distance: %d cm", distance);
			lcd_goto(0,1);
			lcd_write_string(buffer);
			memset(buffer, 0, sizeof buffer); //clear the buffer
	}
	
}


void init_pins(){

	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	PORTB -> PCR[ECHO_PIN_RISING] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[ECHO_PIN_RISING] |= PORT_PCR_MUX(3);  //***setup to be output of TPM1_CH0****
	
	//PORTB->PCR[ECHO_PIN_FALLING] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	//PORTB->PCR[ECHO_PIN_FALLING] |= PORT_PCR_MUX(3);	//***setup to be output of TPM1_CH1****
	
	PORTD->PCR[TRIGGER_PIN] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[TRIGGER_PIN] |= PORT_PCR_MUX(4);	//***setup to be input of TPM0_CH0***
	
}


void init_timer(){
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK | SIM_SCGC6_TPM0_MASK;  //TPM1 AND TPM0 
	// Select clock source in SIM_SOPT
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	TPM0->MOD = 12780u;  //12779.8.... 78 milliseconds
	
	//Setting up for input capture- MSA-MSB(00) ELSB-ELSA(11)
	TPM1->CONTROLS[0].CnSC |= TPM_CnSC_ELSB(1) | TPM_CnSC_ELSA(1); //rising and falling edge
	TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK | TPM_CnSC_CHIE_MASK; //enable interrupts
	
	
	//TPM1->CONTROLS[1].CnSC |= TPM_CnSC_ELSB(0) | TPM_CnSC_ELSA(1); //falling edge
	//TPM1->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK | TPM_CnSC_CHIE_MASK; //enable interrupts
	
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
	
	//NVIC_ClearPendingIRQ(TPM0_IRQn);
	//NVIC_SetPriority(TPM0_IRQn, 1);//
	//NVIC_EnableIRQ(TPM0_IRQn);
	
	__enable_irq();
}


// Interrupt Handlers
//void TPM1_IRQHandler(){
//	static int ctr=0;
//	int rising_edge_time=0;
//	int falling_edge_time=0;
//	

//	// check if input occurred
//	if (TPM1->CONTROLS[0].CnSC & TPM_CnSC_CHF_MASK){		
//		
//		rising_edge_time = TPM1->CONTROLS[0].CnV;
//		TPM1->CONTROLS[0].CnSC |=TPM_CnSC_CHF_MASK; //clear input capture fla1
//	}
//	
//	if (TPM1->CONTROLS[1].CnSC & TPM_CnSC_CHF_MASK){
//		falling_edge_time = TPM1->CONTROLS[1].CnV;
//		TPM1->CONTROLS[1].CnSC |=TPM_CnSC_CHF_MASK;//clear flag
//	}

//	if (TPM1->SC & TPM_SC_TOF_MASK){ 
//				//ctr++; //a timer overflow for the module has occurred.
//				TPM1->SC |= TPM_SC_TOF_MASK ; //clear the interrupt on timer overflow
//		}
//	
//		interval = rising_edge_time - falling_edge_time;	

//}

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



int calculate_time(int interval){
	return interval / 163840;  //interval * prescale/clock frequency
}


int calculate_distance(int time){
	return 17150 * time;   //(343/2) * time * 100
	
}
