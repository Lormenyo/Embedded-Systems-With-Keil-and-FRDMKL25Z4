#include <MKL25Z4.h>>
#include <stdint.h>


#define MASK(X) (1<<X)
#define INPUTA  10 // PTC10, 
#define INPUTB  11 // PTC11
#define SIGNAL_OUTPUT		(22)  	//PTE22
#define BOARD_GREEN_LED (19)  //for PTB19

#define FREQUENCY_01 10
#define FREQUENCY_10 100
#define FREQUENCY_11 1000

#define ON 1
#define OFF 0

int inputA = 0;
int inputB = 0;

void initialise(void);
void systick_clock();
unsigned long millis();
volatile unsigned long counter=0;
void flashLED();
void check_input_statusA();
void check_input_statusB();
void flashLEDother();
void generateWave10();
void generateWave01();
void generateWave11();
void generateWave00();

int main (void) {
	initialise();
	systick_clock();

    while (1)
    {
			flashLED();
			check_input_statusA();
			check_input_statusB();
			flashLEDother();
			generateWave10();
			generateWave01();
			generateWave11();
			generateWave00();
		}
		
	}

	
	
void SysTick_Handler(){
	counter++; //Counting the number of millis
}

void systick_clock(){
 SysTick->LOAD = (20971520u/1000u)-1 ; //configure for every milli sec restart.
 SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk |SysTick_CTRL_TICKINT_Msk; 
}

unsigned long millis(){
return (unsigned long)counter;
}


void initialise(){
	//clock gating to port B,C & E
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTE_MASK |  SIM_SCGC5_PORTC_MASK; ////gate ports B,C & E

	//set up led pins as GPIO --> configure mux to connect pins as GPIO
	PORTB->PCR[BOARD_GREEN_LED ] &= PORT_PCR_MUX_MASK;	//Clear mux
	PORTB->PCR[BOARD_GREEN_LED ] |= PORT_PCR_MUX(1);	//setup to be GPIO
	
	PORTE->PCR[SIGNAL_OUTPUT ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTE->PCR[SIGNAL_OUTPUT ] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT
	
	
		//set up button as GPIO 
	PORTC->PCR[INPUTA] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTC->PCR[INPUTA] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT
	
			//set up button as GPIO 
	PORTC->PCR[INPUTB] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTC->PCR[INPUTB] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT
	
	//SET UP OUTPUT
	PTB->PDDR |= MASK(BOARD_GREEN_LED ) ;
	PTE->PDDR |= MASK(SIGNAL_OUTPUT ) ;
	
	//SET UP INPUT
	PTC->PDDR &= ~MASK(INPUTA) ;		//clear to make input
	PTC->PDDR &= ~MASK(INPUTB);	
	
	PTB->PCOR |= MASK(BOARD_GREEN_LED);
}


void flashLED(){

	static enum red_states {S1, S2} next_state=S1;
	unsigned long current_time;
	static unsigned long last_run=0;

	if ((inputA == OFF) & (inputB == OFF)){

		current_time=millis();
	switch (next_state){
		case S1:
			PTB->PCOR |=MASK(BOARD_GREEN_LED);
			if ((current_time-last_run)>=500){
				last_run=current_time;
				next_state=S2;
			}
			break;
		case S2:
			PTB->PSOR |=MASK(BOARD_GREEN_LED);
			if ((current_time-last_run)>=500){
				last_run=current_time;
				next_state=S1;
			}
			break;
		default:
			next_state =S1;
			break;
	}
	}
}

void flashLEDother(){

	static enum red_states {S1, S2} next_state=S1;
	unsigned long current_time;
	static unsigned long last_run=0;

	if ((inputA == ON) || (inputB == ON)){

		current_time=millis();
	switch (next_state){
		case S1:
			PTB->PCOR |=MASK(BOARD_GREEN_LED);
			if ((current_time-last_run)>=200){
				last_run=current_time;
				next_state=S2;
			}
			break;
		case S2:
			PTB->PSOR |=MASK(BOARD_GREEN_LED);
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
}


void generateWave01(){

	static enum red_states {S1, S2} next_state=S1;
	unsigned long current_time;
	static unsigned long last_run=0;

	if ((inputA == OFF) & (inputB == ON)){

		current_time=millis();
	switch (next_state){
		case S1:
			PTE->PSOR |=MASK(SIGNAL_OUTPUT);
			if ((current_time-last_run)>=5000){
				last_run=current_time;
				next_state=S2;
			}
			break;
		case S2:
			PTE->PCOR |=MASK(SIGNAL_OUTPUT);
			if ((current_time-last_run)>=5000){
				last_run=current_time;
				next_state=S1;
			}
			break;
		default:
			next_state =S1;
			break;
	}
	}
}

void generateWave00(){

	if ((inputA == OFF) & (inputB == OFF)){

			PTE->PCOR |=MASK(SIGNAL_OUTPUT);
}
	}

void generateWave10(){

	static enum red_states {S1, S2} next_state=S1;
	unsigned long current_time;
	static unsigned long last_run=0;

	if ((inputA == ON) & (inputB == OFF)){

		current_time=millis();
	switch (next_state){
		case S1:
			PTE->PSOR |=MASK(SIGNAL_OUTPUT);
			if ((current_time-last_run)>=50000){
				last_run=current_time;
				next_state=S2;
			}
			break;
		case S2:
			PTE->PCOR |=MASK(SIGNAL_OUTPUT);
			if ((current_time-last_run)>=50000){
				last_run=current_time;
				next_state=S1;
			}
			break;
		default:
			next_state =S1;
			break;
	}
	}
}

void generateWave11(){

	static enum red_states {S1, S2} next_state=S1;
	unsigned long current_time;
	static unsigned long last_run=0;

	if ((inputA == ON) & (inputB == ON)){

		current_time=millis();
	switch (next_state){
		case S1:
			PTE->PSOR |=MASK(SIGNAL_OUTPUT);
			if ((current_time-last_run)>=500000){
				last_run=current_time;
				next_state=S2;
			}
			break;
		case S2:
			PTE->PCOR |=MASK(SIGNAL_OUTPUT);
			if ((current_time-last_run)>=500000){
				last_run=current_time;
				next_state=S1;
			}
			break;
		default:
			next_state =S1;
			break;
	}
	}
}



void check_input_statusA(){
	if(PTC->PDIR & MASK(INPUTA)){	//if INPUTA is high 
		inputA=ON;
	}
	else
		inputA=OFF;

}

void check_input_statusB(){
	if(PTC->PDIR & MASK(INPUTB)){	//if INPUTB is high 
		inputB=ON;
	}
	else
		inputB=OFF;
}
