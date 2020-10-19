//will turn on both Red and Blue and external LED on PTD4 alternately
//uses #defined names

#include "mkl25z4.h"

#define MASK(x)			(1UL << x)

void delay(int n){
	for (int j=0; j<n; j++)
		for (int i=0; i<0x180000; i++);
}

int main(){
#define RED_LED	(18)  //B18
#define BLUE_LED  (1) //D1
#define EXT_LED		(4) //PTD4
//clock gating to port B & D
SIM->SCGC5|= ((1UL<<10) | (1<<12))  ; //gate ports B & D
//SIM->SCGC5 |=SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK; //same as above

//set up 2 pins as GPIO --> configure mux to connect pins as GPIO
PORTB->PCR[RED_LED ] &= ~0x700UL;//PORT_PCR_MUX_MASK;	//Clear mux
PORTB->PCR[RED_LED ] |= (1UL<<8); //PORT_PCR_MUX(1);	//setup to be GPIO
PORTD->PCR[BLUE_LED ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
PORTD->PCR[BLUE_LED ] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT
PORTD->PCR[EXT_LED] &= ~PORT_PCR_MUX_MASK;	//Clear mux
PORTD->PCR[EXT_LED ] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT


//set up led to be output
PTB->PDDR |= MASK(RED_LED ) ;
PTD->PDDR |= MASK(BLUE_LED ) ;
PTD->PDDR |= MASK(EXT_LED ) ;

//Following puts the LEDs in a known state
PTB->PSOR =MASK(RED_LED);	//turn off RED
PTD->PSOR =MASK(BLUE_LED);	//turn off BLUE
PTD->PCOR =MASK(EXT_LED);	//turn off EXT (active high)

while (1){ //forever

	PTB->PTOR=MASK(RED_LED); 	//toggle (from off to on)
	delay(1);
	PTB->PTOR=MASK(RED_LED);	//toggle (on to off)
	delay(1);
	PTD->PCOR=MASK(BLUE_LED); // turn on (active low)
	delay(1);
	PTD->PSOR=MASK(BLUE_LED);  //turn off
	delay(1);
	PTD->PSOR=MASK(EXT_LED);	//turn on (active high)
	delay(1);
	PTD->PCOR=MASK(EXT_LED);	//turn off
	delay(1);
	}
}