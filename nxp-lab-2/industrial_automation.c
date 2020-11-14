#include <MKL25Z4.h>
#include <stdint.h>

#define MASK(X) (1<<X)
#define STOPBUTTON  5 // RED BUTTON  PTD5
#define FASTBUTTON  0 // YELLOW BUTTON PTD0
#define FORWARDBUTTON 2 //BLUE BUTTON PTD2
#define BOARD_GREEN_LED (19)  //for PTB19
#define BOARD_RED_LED		(18)  	//PTB18
#define BOARD_BLUE_LED  	(1) 	//PTD1
#define EXT_RED_LED (9) //PTC9
#define EXT_YELLOW_LED (13) //PTA13
#define SEVEN_SEG_D (12) //PTA12
#define SEVEN_SEG_C (4) //PTA4
#define SEVEN_SEG_E (5) //PTA5
#define SEVEN_SEG_G (8) //PTC8

// Defining machine states
#define STOP 0
#define GO 1
#define FORWARD 0
#define BACKWARD 1

// Defining intervals 
#define ONE_SECOND 1000

#define SLOW_TIME_INTERVAL 100
#define FAST_TIME_INTERVAL 50

int speed=SLOW_TIME_INTERVAL;
int direction_status=FORWARD;
int machine_status = STOP;

void initialise(void);
void systick_clock();
void sevenSegForward(void);
void sevenSegReverse(void);
void check_machine_status();
void check_direction_status();
void check_speed_status();
void flashDuringStop();
void stopFlashDuringGo();
void turnAllSegOff();
void whiteFlashDuringFastForward();
void whiteFlashDuringForward();
void flashExtYellowDuringForward();
void flashExtRedDuringForward();
void flashBoardBlueDuringReverse();
void flashBoardBlueDuringFastReverse();
void flashExtRedDuringReverse();
void flashExtYellowDuringReverse();


unsigned long millis();
volatile unsigned long counter=0;
volatile int32_t timeDelay;

unsigned long interval=1000; //1 sec

int main (void) {
	initialise();
	systick_clock();

    while (1)
    {

			sevenSegForward();
			check_speed_status();
			sevenSegReverse();
			check_direction_status();
			turnAllSegOff();
			flashDuringStop();
			check_machine_status();
			stopFlashDuringGo();
			whiteFlashDuringFastForward();
			whiteFlashDuringForward();
			flashExtYellowDuringForward();
			flashExtRedDuringForward();
			flashBoardBlueDuringReverse();
			flashBoardBlueDuringFastReverse();
			flashExtRedDuringReverse();
			flashExtYellowDuringReverse();
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
	//clock gating to port A,B,C & D
	SIM->SCGC5 |=SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK |  SIM_SCGC5_PORTC_MASK; ////gate ports A,B,C & D

	//set up led pins as GPIO --> configure mux to connect pins as GPIO
	PORTB->PCR[BOARD_RED_LED ] &= PORT_PCR_MUX_MASK;	//Clear mux
	PORTB->PCR[BOARD_RED_LED ] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PORTD->PCR[BOARD_BLUE_LED ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[BOARD_BLUE_LED ] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT
	PORTC->PCR[EXT_RED_LED] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTC->PCR[EXT_RED_LED ] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT
	PORTA->PCR[EXT_YELLOW_LED] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTA->PCR[EXT_YELLOW_LED ] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT
	

	//set up seven segments as gpio
	PORTA->PCR[SEVEN_SEG_D ] &= PORT_PCR_MUX_MASK;	//Clear mux
	PORTA->PCR[SEVEN_SEG_D ] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PORTA->PCR[SEVEN_SEG_C ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTA->PCR[SEVEN_SEG_C ] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT
	PORTA->PCR[SEVEN_SEG_E] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTA->PCR[SEVEN_SEG_E ] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT
	PORTC->PCR[SEVEN_SEG_G] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTC->PCR[SEVEN_SEG_G ] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT
	
	//set up button as GPIO 
	PORTD->PCR[FORWARDBUTTON] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[FORWARDBUTTON] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT
	PORTD->PCR[FASTBUTTON] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[FASTBUTTON] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT
	PORTD->PCR[STOPBUTTON] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[STOPBUTTON] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT

	
	//enable internal pullup for buttons
	//PORTD->PCR[FORWARDBUTTON] |= PORT_PCR_PE_MASK;
	//PORTD->PCR[FASTBUTTON] |= PORT_PCR_PE_MASK;
	//PORTD->PCR[STOPBUTTON] |= PORT_PCR_PE_MASK;
	
	//set up led to be output
	PTB->PDDR |= MASK(BOARD_RED_LED ) ;
	PTD->PDDR |= MASK(BOARD_BLUE_LED ) ;
	PTB->PDDR |= MASK(BOARD_GREEN_LED ) ;
	PTC->PDDR |= MASK(EXT_RED_LED ) ;
	PTA->PDDR |= MASK(EXT_YELLOW_LED ) ;
	
	//set up seven seg to be output
	PTA->PDDR |= MASK(SEVEN_SEG_D ) ;
	PTA->PDDR |= MASK(SEVEN_SEG_C ) ;
	PTA->PDDR |= MASK(SEVEN_SEG_E ) ;
	PTC->PDDR |= MASK(SEVEN_SEG_G ) ;
	
	
	//set up buttons to be input
	PTD->PDDR &= ~MASK(FORWARDBUTTON) ;		//clear to make input
	PTD->PDDR &= ~MASK(FASTBUTTON);	
	PTD->PDDR &= ~MASK(STOPBUTTON);	
		
	//Following puts the LEDs in a known state
	PTB->PSOR =MASK(BOARD_RED_LED);	//turn off RED
	PTD->PSOR =MASK(BOARD_BLUE_LED);	//turn off BLUE
	PTC->PCOR =MASK(EXT_RED_LED);	//turn off EXT (active high)
	PTA->PCOR =MASK(EXT_YELLOW_LED);	//turn off EXT (active high)
}



//=======================================
void sevenSegForward(void){
	//flash seven segment display clockwise(g->c->d->e)
	static enum seg_states {S1, S2, S3, S4, S5, S6, S7, S8} next_state=S1;
	unsigned long current_time;
	static unsigned long last_run=0;

	if(machine_status & (direction_status==FORWARD)){

	current_time=millis();
	switch (next_state){
		case S1:
			PTC->PSOR |=MASK(SEVEN_SEG_G); //turn seg G on
			if ((current_time-last_run)>=speed){
				last_run=current_time;
				next_state=S2;
			}
			break;
		case S2:
			PTC->PCOR |=MASK(SEVEN_SEG_G); //turn seg G off
			if ((current_time-last_run)>=speed){
				last_run=current_time;
				next_state=S3;
			}
			break;
		case S3:
			PTA->PSOR |=MASK(SEVEN_SEG_C); //turn seg C on
			if ((current_time-last_run)>=speed){
				last_run=current_time;
				next_state=S4;
			}
			break;
		case S4:
			PTA->PCOR |=MASK(SEVEN_SEG_C); //turn seg C off
			if ((current_time-last_run)>=speed){
				last_run=current_time;
				next_state=S5;
			}
			break;
		case S5:
			PTA->PSOR |=MASK(SEVEN_SEG_D); //turn seg D on
			if ((current_time-last_run)>=speed){
				last_run=current_time;
				next_state=S6;
			}
			break;
		case S6:
			PTA->PCOR |=MASK(SEVEN_SEG_D); //turn seg D off
			if ((current_time-last_run)>=speed){
				last_run=current_time;
				next_state=S7;
			}
			break;
		case S7:
			PTA->PSOR |=MASK(SEVEN_SEG_E); //turn seg E on
			if ((current_time-last_run)>=speed){
				last_run=current_time;
				next_state=S8;
			}
			break;
		case S8:
			PTA->PCOR |=MASK(SEVEN_SEG_E); //turn seg E off
			if ((current_time-last_run)>=speed){
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


void sevenSegReverse(void){
		//flash seven segment display anticlockwise(e->d->c->g)
	static enum seg_states {S1, S2, S3, S4, S5, S6, S7, S8} next_state=S1;
	unsigned long current_time;
	static unsigned long last_run=0;

	
	if((machine_status==GO) & (direction_status==BACKWARD)){
		current_time=millis();
		switch(next_state){
			case S1: 
					PTA->PCOR |=MASK(SEVEN_SEG_E); //turn seg E on
					if ((current_time-last_run)>=speed){
							last_run=current_time;
							next_state=S2;
					}
			case S2: 
					PTA->PSOR |=MASK(SEVEN_SEG_E); //turn seg E off
					if ((current_time-last_run)>=speed){
					last_run=current_time;
					next_state=S3;
					}
				break;
			case S3: 
					PTA->PCOR |=MASK(SEVEN_SEG_D); //turn seg D on
					if ((current_time-last_run)>=speed){
						last_run=current_time;
						next_state=S4;
				}
			break;
			case S4: 
				PTA->PSOR |=MASK(SEVEN_SEG_D); //turn seg D off
				if ((current_time-last_run)>=speed){
					last_run=current_time;
					next_state=S5;
				}
			break;
			case S5:
				PTA->PCOR |=MASK(SEVEN_SEG_C); //turn seg C on
				if ((current_time-last_run)>=speed){
					last_run=current_time;
					next_state=S6;
				}
				break;
			case S6:
				PTA->PSOR |=MASK(SEVEN_SEG_C); //turn seg C off
				if ((current_time-last_run)>=speed){
							last_run=current_time;
							next_state=S7;
					}
				break;
			case S7:
					PTC->PCOR |=MASK(SEVEN_SEG_G); //turn seg G on
					if ((current_time-last_run)>=speed){
						last_run=current_time;
						next_state=S8;
					}
					break;
			case S8:
					PTC->PSOR |=MASK(SEVEN_SEG_G); //turn seg G off
					if ((current_time-last_run)>=speed){
						last_run=current_time;
						next_state=S1;
					}
					break;
			default: 
						PTC->PCOR |=MASK(SEVEN_SEG_G);
						PTA->PCOR |=MASK(SEVEN_SEG_C);
						PTA->PCOR |=MASK(SEVEN_SEG_D);
						PTA->PCOR |=MASK(SEVEN_SEG_E);
				next_state=S1; break;
		}
	}
}

void turnAllSegOff(){
	if (machine_status==STOP){
		PTC->PCOR |=MASK(SEVEN_SEG_G);
		PTA->PCOR |=MASK(SEVEN_SEG_C);
		PTA->PCOR |=MASK(SEVEN_SEG_D);
		PTA->PCOR |=MASK(SEVEN_SEG_E);
	}
}



void flashBoardBlueDuringReverse(){ //ACTIVE LOW	
	static enum blue_states {S1, S2, S3, S4} next_state=S1;
	unsigned long current_time;
	static unsigned long last_run=0;

	if (machine_status==GO & direction_status==BACKWARD){

		current_time=millis();
		switch (next_state){
		case S1:
			PTD->PCOR |= MASK(BOARD_BLUE_LED); 	//on
			if ((current_time-last_run)>=600){
				last_run=current_time;
				next_state=S2;
			}
			break;
		case S2:
			PTD->PSOR=MASK(BOARD_BLUE_LED);	//OFF
			if ((current_time-last_run)>=100){
				last_run=current_time;
				next_state=S3;
			}
			break;
		case S3:
			PTD->PCOR |= MASK(BOARD_BLUE_LED); 	//on
			if ((current_time-last_run)>=600){
				last_run=current_time;
				next_state=S4;
			}
			break;
		case S4:
			PTD->PSOR=MASK(BOARD_BLUE_LED);	//OFF
			if ((current_time-last_run)>=100){
				last_run=current_time;
				next_state=S1;
			}
			break;
		default:
			next_state=S1;
			break;
		}
	}
}

void flashBoardBlueDuringFastReverse(){ //ACTIVE LOW	
	static enum blue_states {S1, S2, S3, S4, S5, S6} next_state=S1;
	unsigned long current_time;
	static unsigned long last_run=0;

	if (machine_status==GO & direction_status==BACKWARD & speed==FAST_TIME_INTERVAL){

		current_time=millis();
		switch (next_state){
		case S1:
			PTD->PCOR |= MASK(BOARD_BLUE_LED); 	//on
			if ((current_time-last_run)>=200){
				last_run=current_time;
				next_state=S2;
			}
			break;
		case S2:
			PTD->PSOR=MASK(BOARD_BLUE_LED);	//OFF
			if ((current_time-last_run)>=100){
				last_run=current_time;
				next_state=S3;
			}
			break;
		case S3:
			PTD->PCOR |= MASK(BOARD_BLUE_LED); 	//on
			if ((current_time-last_run)>=400){
				last_run=current_time;
				next_state=S4;
			}
			break;
		case S4:
			PTD->PSOR=MASK(BOARD_BLUE_LED);	//OFF
			if ((current_time-last_run)>=100){
				last_run=current_time;
				next_state=S5;
			}
			break;
		case S5:
			PTD->PCOR |= MASK(BOARD_BLUE_LED); 	//on
			if ((current_time-last_run)>=600){
				last_run=current_time;
				next_state=S6;
			}
			break;
		case S6:
			PTD->PSOR=MASK(BOARD_BLUE_LED);	//OFF
			if ((current_time-last_run)>=100){
				last_run=current_time;
				next_state=S1;
			}
			break;
		default:
			next_state=S1;
			break;
		}
	}
}

static void flashBoardWhite(int t){ //ACTIVE LOW
	PTD->PCOR |= MASK(BOARD_BLUE_LED); 	//on)
	PTB->PCOR |= MASK(BOARD_RED_LED);	//on)
	PTB->PCOR |= MASK(BOARD_GREEN_LED); 	//on)

	PTD->PTOR=MASK(BOARD_BLUE_LED);	//OFF
	PTB->PTOR=MASK(BOARD_RED_LED);	//OFF
	PTB->PTOR=MASK(BOARD_GREEN_LED);	//OFF
}


void flashDuringStop(){
	static enum red_states {S1, S2, S3, S4} next_state=S1;
	unsigned long current_time;
	static unsigned long last_run=0;

	if(machine_status==STOP){ //when the machine stops
	current_time=millis();
	switch (next_state){
		case S1:
			PTC->PSOR |=MASK(EXT_RED_LED); //turn external red led on
			if ((current_time-last_run)>=ONE_SECOND){
				last_run=current_time;
				next_state=S2;
			}
			break;
		case S2:
			PTC->PCOR |=MASK(EXT_RED_LED); //turn external red led off
			if ((current_time-last_run)>=ONE_SECOND){
				last_run=current_time;
				next_state=S3;
			}
			break;
		case S3:
			PTA->PSOR |=MASK(EXT_YELLOW_LED); //turn external yellow led on
			if ((current_time-last_run)>=ONE_SECOND){
				last_run=current_time;
				next_state=S4;
			}
			break;
		case S4:
			PTA->PCOR |=MASK(EXT_YELLOW_LED); //turn external yellow led off
			if ((current_time-last_run)>=ONE_SECOND){
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

void stopFlashDuringGo(){
	if(machine_status){
			PTC->PCOR |= MASK(EXT_RED_LED);	//turn off EXT (active high)
			PTA->PCOR |= MASK(EXT_YELLOW_LED);		//turn off EXT (active high)
	}
}

void whiteFlashDuringForward(){
	static enum red_states {S1, S2, S3, S4, S5} next_state=S1;
	unsigned long current_time;
	static unsigned long last_run=0;
	
	if((machine_status==GO) & (direction_status==FORWARD)){
		current_time=millis();
	switch (next_state){
		case S1:
			PTB->PCOR |= MASK(BOARD_GREEN_LED);
			PTB->PCOR |= MASK(BOARD_RED_LED);
			PTD->PCOR |= MASK(BOARD_BLUE_LED);
			if ((current_time-last_run)>=150){
				last_run=current_time;
				next_state=S2;
			}
			break;
		case S2:
			PTB->PSOR |= MASK(BOARD_GREEN_LED);
			PTB->PSOR |= MASK(BOARD_RED_LED);
			PTD->PSOR |= MASK(BOARD_BLUE_LED);
			if ((current_time-last_run)>=50){
				last_run=current_time;
				next_state=S3;
			}
			break;
			case S3:
			PTB->PCOR |= MASK(BOARD_GREEN_LED);
			PTB->PCOR |= MASK(BOARD_RED_LED);
			PTD->PCOR |= MASK(BOARD_BLUE_LED);
			if ((current_time-last_run)>=150){
				last_run=current_time;
				next_state=S4;
			}
			break;
		case S4:
			PTB->PSOR |= MASK(BOARD_GREEN_LED);
			PTB->PSOR |= MASK(BOARD_RED_LED);
			PTD->PSOR |= MASK(BOARD_BLUE_LED);
			if ((current_time-last_run)>=150){
				last_run=current_time;
				next_state=S5;
			}
			break;
		case S5:
			PTB->PSOR |= MASK(BOARD_GREEN_LED);
			PTB->PSOR |= MASK(BOARD_RED_LED);
			PTD->PSOR |= MASK(BOARD_BLUE_LED);
			if ((current_time-last_run)>=150){
				last_run=current_time;
				next_state=S1;
			}
			break;
		default:
			next_state =S1;
			break;
	}
	}
	else{
			PTB->PSOR |= MASK(BOARD_GREEN_LED);
			PTB->PSOR |= MASK(BOARD_RED_LED);
			PTD->PSOR |= MASK(BOARD_BLUE_LED);
	}
}

void whiteFlashDuringFastForward(){
	static enum red_states {S1, S2, S3, S4, S5, S6} next_state=S1;
	unsigned long current_time;
	static unsigned long last_run=0;
	
	if((machine_status==GO) & (direction_status==FORWARD) & (speed==FAST_TIME_INTERVAL)){
		current_time=millis();
	switch (next_state){
		case S1:
			PTB->PCOR |= MASK(BOARD_GREEN_LED);
			PTB->PCOR |= MASK(BOARD_RED_LED);
			PTD->PCOR |= MASK(BOARD_BLUE_LED);
			if ((current_time-last_run)>=200){
				last_run=current_time;
				next_state=S2;
			}
			break;
		case S2:
			PTB->PSOR |= MASK(BOARD_GREEN_LED);
			PTB->PSOR |= MASK(BOARD_RED_LED);
			PTD->PSOR |= MASK(BOARD_BLUE_LED);
			if ((current_time-last_run)>=200){
				last_run=current_time;
				next_state=S3;
			}
			break;
			case S3:
			PTB->PCOR |= MASK(BOARD_GREEN_LED);
			PTB->PCOR |= MASK(BOARD_RED_LED);
			PTD->PCOR |= MASK(BOARD_BLUE_LED);
			if ((current_time-last_run)>=300){
				last_run=current_time;
				next_state=S4;
			}
			break;
		case S4:
			PTB->PSOR |= MASK(BOARD_GREEN_LED);
			PTB->PSOR |= MASK(BOARD_RED_LED);
			PTD->PSOR |= MASK(BOARD_BLUE_LED);
			if ((current_time-last_run)>=200){
				last_run=current_time;
				next_state=S5;
			}
			break;
		case S5:
			PTB->PCOR |= MASK(BOARD_GREEN_LED);
			PTB->PCOR |= MASK(BOARD_RED_LED);
			PTD->PCOR |= MASK(BOARD_BLUE_LED);
			if ((current_time-last_run)>=500){
				last_run=current_time;
				next_state=S6;
			}
			break;
		case S6:
			PTB->PSOR |= MASK(BOARD_GREEN_LED);
			PTB->PSOR |= MASK(BOARD_RED_LED);
			PTD->PSOR |= MASK(BOARD_BLUE_LED);
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
	else{
			PTB->PSOR |= MASK(BOARD_GREEN_LED);
			PTB->PSOR |= MASK(BOARD_RED_LED);
			PTD->PSOR |= MASK(BOARD_BLUE_LED);
	}
}

void flashExtYellowDuringForward(){
		static enum red_states {S1, S2} next_state=S1;
	unsigned long current_time;
	static unsigned long last_run=0;

	if ((machine_status==GO) & (direction_status==FORWARD)){

		current_time=millis();
	switch (next_state){
		case S1:
			PTA->PSOR |=MASK(EXT_YELLOW_LED);
			if ((current_time-last_run)>=ONE_SECOND){
				last_run=current_time;
				next_state=S2;
			}
			break;
		case S2:
			PTA->PCOR |=MASK(EXT_YELLOW_LED);
			if ((current_time-last_run)>=ONE_SECOND){
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

void flashExtYellowDuringReverse(){
		static enum red_states {S1, S2} next_state=S1;
	unsigned long current_time;
	static unsigned long last_run=0;

	if ((machine_status==GO) & (direction_status==BACKWARD)){

		current_time=millis();
	switch (next_state){
		case S1:
			PTA->PSOR |=MASK(EXT_YELLOW_LED);
			if ((current_time-last_run)>=400){
				last_run=current_time;
				next_state=S2;
			}
			break;
		case S2:
			PTA->PCOR |=MASK(EXT_YELLOW_LED);
			if ((current_time-last_run)>=400){
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

void flashExtRedDuringForward(){
		static enum red_states {S1, S2} next_state=S1;
	unsigned long current_time;
	static unsigned long last_run=0;

	if ((machine_status==GO) & (direction_status==FORWARD)){

		current_time=millis();
	switch (next_state){
		case S1:
			PTC->PSOR |= MASK(EXT_RED_LED);
			if ((current_time-last_run)>=600){
				last_run=current_time;
				next_state=S2;
			}
			break;
		case S2:
			PTC->PCOR |=MASK(EXT_RED_LED);
			if ((current_time-last_run)>=600){
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

void flashExtRedDuringReverse(){
		static enum red_states {S1, S2} next_state=S1;
	unsigned long current_time;
	static unsigned long last_run=0;

	if ((machine_status==GO) & (direction_status==BACKWARD)){

		current_time=millis();
	switch (next_state){
		case S1:
			PTC->PSOR |= MASK(EXT_RED_LED);
			if ((current_time-last_run)>=ONE_SECOND){
				last_run=current_time;
				next_state=S2;
			}
			break;
		case S2:
			PTC->PCOR |=MASK(EXT_RED_LED);
			if ((current_time-last_run)>=ONE_SECOND){
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



void check_direction_status(){
	if(PTD->PDIR & MASK(FORWARDBUTTON)){	//if FORWARDBUTTON is high
		direction_status=BACKWARD;
	}
	else
		direction_status=FORWARD;
}

void check_machine_status(){
	if(PTD->PDIR & MASK(STOPBUTTON)){	//if STOPBUTTON is high 
		machine_status=GO;
	}
	else
		machine_status=STOP;
}

void check_speed_status(){
	if(PTD->PDIR & MASK(FASTBUTTON)){	//if FASTBUTTON is high
		speed = FAST_TIME_INTERVAL ;
	}
	else
		speed = SLOW_TIME_INTERVAL;
}


