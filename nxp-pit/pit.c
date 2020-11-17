#include "mkl25z4.h"

#define MASK(x)			(1UL << x)
#define RED_LED		(18)  	//B18

void init_led();
void init_pit();

int num_of_toggles = 0;

int main(){
	init_led();
	init_pit();


	while(1){}
}

void init_pit(){
	//Enable clock to PIT module
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
 //Enable module, freeze timers in debug mode
	PIT->MCR &= ~PIT_MCR_MDIS_MASK; //enable mdis
//	PIT->MCR |= PIT_MCR_FRZ_MASK;
	
	//Initialize PIT0 to count down from starting_value
	PIT->CHANNEL[0].LDVAL =0x3FFFFF;   //every 400ms 
	//No chaining of timers
	PIT->CHANNEL[0].TCTRL &= PIT_TCTRL_CHN_MASK;
	
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
	
	//Let the PIT channel generate interrupt requests
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;
	
	
	//Initialize PIT0 to count down from starting_value
	PIT->CHANNEL[1].LDVAL =0xFFFFF;   //every 100ms 
	//No chaining of timers
	PIT->CHANNEL[1].TCTRL &= PIT_TCTRL_CHN_MASK;
	
	
	
	//Let the PIT channel generate interrupt requests
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TIE_MASK;
	
	NVIC_SetPriority(PIT_IRQn, 3); 
	//Clear any pending IRQ from PIT
	NVIC_ClearPendingIRQ(PIT_IRQn); 
	//Enable the PIT interrupt in the NVIC
	NVIC_EnableIRQ(PIT_IRQn);	


}


void PIT_IRQHandler(){
	
	//Determine which channel triggered interrupt
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {		
		PTB->PTOR |=MASK(RED_LED);	//toggle RED
		//Clear interrupt request flag for channel
		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
		num_of_toggles++;
		
		if (num_of_toggles == 10){
			PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK;
			PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;
			num_of_toggles = 0;
		}
	}
	
	
	if (PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) {		
		PTB->PTOR |=MASK(RED_LED);	//toggle RED
		//Clear interrupt request flag for channel
		PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK;
		num_of_toggles++;
		
		if (num_of_toggles == 10){
			PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
			PIT->CHANNEL[1].TCTRL &= ~PIT_TCTRL_TEN_MASK;
			num_of_toggles = 0;
		}
	}
	
	
}


void init_led(){


		//clock gating to port B & D
		SIM->SCGC5 |=SIM_SCGC5_PORTB_MASK ; //same as above

		//set up 2 pins as GPIO --> configure mux to connect pins as GPIO
		PORTB->PCR[RED_LED ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
		PORTB->PCR[RED_LED ] |= PORT_PCR_MUX(1);	//setup to be GPIO



		//set up led to be output
		PTB->PDDR |= MASK(RED_LED ) ;

		PTB->PSOR |=MASK(RED_LED);
}
