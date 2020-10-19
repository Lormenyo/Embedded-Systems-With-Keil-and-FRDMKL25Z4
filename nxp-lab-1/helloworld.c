#define BLINKY
//#define FLASHY
//#define MODULAR_FLASHY
//#define RESPONSIVE_FLASHY
#define EXT 3  //for PTD3
//void delay(int delay);


//#define HALF_SEC
//#define MILLI_SEC
//#define interrupt_version
//#define millis_version

#ifdef BLINKY

#include "MKL25Z4.h"   ///---------
#define MASK(x)			(1UL << x)


int main(){
#define LED_RED	(18)  //PTB18
#define LED_BLUE (1) //PTD1
#define LED_GREEN (19) //PTB19
#define LED_EXTERNAL (5) //PTA5
	
	
//clock gating to port B
SIM->SCGC5 |=SIM_SCGC5_PORTB_MASK;
	
//clock gating to PORT D
SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	
//clock gating to PORT A
SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	

//setting up as GPIO
PORTB->PCR[LED_RED ] &= ~PORT_PCR_MUX_MASK;	//Clear mux 0x700;//
PORTB->PCR[LED_RED ] |= PORT_PCR_MUX(1);			//setup to be GPIO (1UL<<8); //
	
PORTB->PCR[LED_GREEN ] &= ~PORT_PCR_MUX_MASK;	//Clear mux 0x700;//
PORTB->PCR[LED_GREEN] |= PORT_PCR_MUX(1);

PORTD->PCR[LED_BLUE ] &= ~PORT_PCR_MUX_MASK;	//Clear mux 0x700;//
PORTD->PCR[LED_BLUE] |= PORT_PCR_MUX(1);

PORTB->PCR[LED_EXTERNAL ] &= ~PORT_PCR_MUX_MASK;	//Clear mux 0x700;//
PORTA->PCR[LED_EXTERNAL] |= PORT_PCR_MUX(1);

//set up led to be output
PTB->PDDR |= MASK(LED_RED ) ;
PTB->PDDR |= MASK(LED_GREEN );
PTD->PDDR |= MASK(LED_BLUE ) ;
PTA->PDDR |= MASK(LED_EXTERNAL ) ;


while (1){ //forever
	
	int i;

	PTD->PCOR=MASK(LED_BLUE);
	for (i=0; i<0x1800000; i++);
	PTD->PSOR=MASK(LED_BLUE);
	for (i=0; i<0x280000; i++);

	PTB->PCOR=MASK(LED_RED);
	for (i=0; i<0x1800000; i++);
	PTB->PSOR=MASK(LED_RED);
	for (i=0; i<0x280000; i++);
	
	PTB->PCOR=MASK(LED_GREEN);
	for (i=0; i<0x1800000; i++);
	PTB->PSOR=MASK(LED_GREEN);
	for (i=0; i<0x280000; i++);
	
	PTA->PSOR=MASK(LED_EXTERNAL);
	for (i=0; i<0x1800000; i++);
	PTA->PCOR=MASK(LED_EXTERNAL);
	for (i=0; i<0x280000; i++);
}
}

#endif

#ifdef FLASHY

#include "MKL25Z4.h"   ///---------
#define MASK(x)			(1UL << x)


void delay(int n);
static void flashR(int t);
static void flashB(int t);

int main(){
#define RED_LED		(18)  	//B18
#define BLUE_LED  	(1) 	//D1
#define EXT_LED		(4) 	//PTD4
#define PIN1		(3)		//PTD3
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

//configure input
PORTD->PCR[PIN1] &= ~PORT_PCR_MUX_MASK;	//Clear mux
PORTD->PCR[PIN1 ] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT


//set up led to be output
PTB->PDDR |= MASK(RED_LED ) ;
PTD->PDDR |= MASK(BLUE_LED ) ;
PTD->PDDR |= MASK(EXT_LED ) ;
PTD->PDDR &= ~MASK(PIN1 ) ;		//clear to make input

//Following puts the LEDs in a known state
PTB->PSOR =MASK(RED_LED);	//turn off RED
PTD->PSOR =MASK(BLUE_LED);	//turn off BLUE
PTD->PCOR =MASK(EXT_LED);	//turn off EXT (active high)

while (1){ //forever
	if(PTD->PDIR & MASK(PIN1)){	//if PIN1 is high
		flashR(3);
		flashR(1);
		delay(4);
		flashR(3);
		flashB(1);
		delay(4);
		flashB(3);
		flashB(1);
		delay(4);
	}
	else{
		int duration=10;
		PTB->PTOR=MASK(RED_LED); 	//toggle (from off to on)
		delay(duration);
		PTB->PTOR=MASK(RED_LED);	//toggle (on to off)
		delay(duration);
		PTD->PCOR|=MASK(BLUE_LED); // turn on (active low)
		delay(duration);
		PTD->PSOR|=MASK(BLUE_LED);  //turn off
		delay(duration);
		PTD->PSOR|=MASK(EXT_LED);	//turn on (active high)
		delay(duration);
		PTD->PCOR|=MASK(EXT_LED);	//turn off
		delay(duration);
	}
	}
}

//other functions
void delay(int n){
	int j;
	int i;
	for (j=0; j<n; j++)
		//for (int i=0; i<0x180000; i++);
		for (i=0; i<0x30000; i++);
}

static void flashR(int t){
	PTB->PCOR=MASK(RED_LED); 	//on)
	delay(t);
	PTB->PTOR=MASK(RED_LED);	//OFF
	delay(t);
}
static void flashB(int t){
	PTD->PCOR=MASK(BLUE_LED); 	//on)
	delay(t);
	PTD->PTOR=MASK(BLUE_LED);	//OFF
	delay(t);
}
#endif

#ifdef MODULAR_FLASHY

//use PTD3 as input, with ext pull down
//if set high, flashes RR_RB_BB_pattern, else default_pattern.
//uses tasks

#include "mkl25z4.h"

#define MASK(x)			(1UL << x)

void delay(int n);
void default_pattern();
void RR_RB_BB_pattern();
void initialize();

int main(){
#define RED_LED		(18)  	//B18
#define BLUE_LED  	(1) 	//D1
#define EXT_LED		(4) 	//PTD4
#define PIN1		(3)		//PTD3

initialize();
while (1){ //forever
	if(PTD->PDIR & MASK(PIN1)){	//if PIN1 is high
		RR_RB_BB_pattern();
	}
	else{
		default_pattern();
	}
	}
}

void initialize(){
	//clock gating to port B & D
	SIM->SCGC5 |=SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK; ////gate ports B & D

	//set up 2 pins as GPIO --> configure mux to connect pins as GPIO
	PORTB->PCR[RED_LED ] &= ~0x700UL;//PORT_PCR_MUX_MASK;	//Clear mux
	PORTB->PCR[RED_LED ] |= (1UL<<8); //PORT_PCR_MUX(1);	//setup to be GPIO
	PORTD->PCR[BLUE_LED ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[BLUE_LED ] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT
	PORTD->PCR[EXT_LED] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[EXT_LED ] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT

	//configure input
	PORTD->PCR[PIN1] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[PIN1 ] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT
	//pulldown is configured with external resistor.
	//include following to enable internal pull-up resistor.
	//PORTD->PCR[PIN1] |=0x2;		//or PORT_PCR_PE_MASK;


	//set up led to be output
	PTB->PDDR |= MASK(RED_LED ) ;
	PTD->PDDR |= MASK(BLUE_LED ) ;
	PTD->PDDR |= MASK(EXT_LED ) ;
	PTD->PDDR &= ~MASK(PIN1 ) ;		//clear to make input

	//Following puts the LEDs in a known state
	PTB->PSOR =MASK(RED_LED);	//turn off RED
	PTD->PSOR =MASK(BLUE_LED);	//turn off BLUE
	PTD->PCOR =MASK(EXT_LED);	//turn off EXT (active high)

}

//other functions
void delay(int n){
	int j;
	int i;
	for ( j=0; j<n; j++){
		for ( i=0; i<0x30000; i++);
	}
		
}

static void flashR(int t){
	PTB->PCOR=MASK(RED_LED); 	//on)
	delay(t);
	PTB->PTOR=MASK(RED_LED);	//OFF
	delay(t);
}
static void flashB(int t){
	PTD->PCOR=MASK(BLUE_LED); 	//on)
	delay(t);
	PTD->PTOR=MASK(BLUE_LED);	//OFF
	delay(t);
}
void RR_RB_BB_pattern(){
	flashR(3);
	flashR(1);
	delay(4);
	flashR(3);
	flashB(1);
	delay(4);
	flashB(3);
	flashB(1);
	delay(4);
}

void default_pattern(){
int duration=10;
	PTB->PTOR=MASK(RED_LED); 	//toggle (from off to on)
	delay(duration);
	PTB->PTOR=MASK(RED_LED);	//toggle (on to off)
	delay(duration);
	PTD->PCOR|=MASK(BLUE_LED); // turn on (active low)
	delay(duration);
	PTD->PSOR|=MASK(BLUE_LED);  //turn off
	delay(duration);
	PTD->PSOR|=MASK(EXT_LED);	//turn on (active high)
	delay(duration);
	PTD->PCOR|=MASK(EXT_LED);	//turn off
	delay(duration);
}

#endif


#ifdef RESPONSIVE_FLASHY

//use PTD3 as input, with ext pull down
//if set high, flashes RR_RB_BB_pattern, else default_pattern.
//uses tasks

#include "mkl25z4.h"

#define MASK(x)			(1UL << x)

void delay(int n);
void default_pattern();
void RR_RB_BB_pattern();
void initialize();
void check_ptd4();

int g_ptd3_status=0;

int main(){
#define RED_LED		(18)  	//B18
#define BLUE_LED  	(1) 	//D1
#define EXT_LED		(4) 	//PTD4
#define PIN1		(3)		//PTD3

initialize();
while (1){ //forever

		RR_RB_BB_pattern();
		default_pattern();
		check_ptd4();
	}
}

void initialize(){
	//clock gating to port B & D
	SIM->SCGC5 |=SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK; ////gate ports B & D

	//set up 2 pins as GPIO --> configure mux to connect pins as GPIO
	PORTB->PCR[RED_LED ] &= ~0x700UL;//PORT_PCR_MUX_MASK;	//Clear mux
	PORTB->PCR[RED_LED ] |= (1UL<<8); //PORT_PCR_MUX(1);	//setup to be GPIO
	PORTD->PCR[BLUE_LED ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[BLUE_LED ] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT
	PORTD->PCR[EXT_LED] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[EXT_LED ] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT

	//configure input
	PORTD->PCR[PIN1] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[PIN1 ] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT
	//pulldown is configured with external resistor.
	//include following to enable internal pull-up resistor.
	//PORTD->PCR[PIN1] |=0x2;		//or PORT_PCR_PE_MASK;


	//set up led to be output
	PTB->PDDR |= MASK(RED_LED ) ;
	PTD->PDDR |= MASK(BLUE_LED ) ;
	PTD->PDDR |= MASK(EXT_LED ) ;
	PTD->PDDR &= ~MASK(PIN1 ) ;		//clear to make input

	//Following puts the LEDs in a known state
	PTB->PSOR =MASK(RED_LED);	//turn off RED
	PTD->PSOR =MASK(BLUE_LED);	//turn off BLUE
	PTD->PCOR =MASK(EXT_LED);	//turn off EXT (active high)

}

//other functions
void delay(int n){
	int j,i;
	for (j=0; j<n; j++)
		//for (int i=0; i<0x180000; i++);
		for (i=0; i<0x30000; i++);
}

static void flashR(int t){
	PTB->PCOR=MASK(RED_LED); 	//on)
	delay(t);
	PTB->PTOR=MASK(RED_LED);	//OFF
	delay(t);
}
static void flashB(int t){
	PTD->PCOR=MASK(BLUE_LED); 	//on)
	delay(t);
	PTD->PTOR=MASK(BLUE_LED);	//OFF
	delay(t);
}

void check_ptd4(){
	if(PTD->PDIR & MASK(PIN1)){	//if PIN1 is high
		g_ptd3_status=1;
	}
	else
		g_ptd3_status=0;

}
enum RBRRstates {S1, S2, S3, S4, S5, S6, S7, S8, S9};

void RR_RB_BB_pattern(){
	static enum RBRRstates next_state=S1;
	if(g_ptd3_status){
		switch(next_state){
			case S1: flashR(3);next_state=S2; break;
			case S2: flashR(1);next_state=S3; break;
			case S3: delay(4);next_state=S4; break;
			case S4: flashR(3);next_state=S5; break;
			case S5: flashB(1);next_state=S6; break;
			case S6: delay(4);next_state=S7; break;
			case S7: flashB(3);next_state=S8; break;
			case S8: flashB(1);next_state=S9; break;
			case S9: delay(4);next_state=S1; break;
			default: next_state=S1; break;
		}
	}
}

enum default_states{s1, s2, s3, s4, s5, s6,s7,s8,s9,s10, s11};
void default_pattern(){
	int duration=10;
	static enum default_states d_next_state=s1;
	if (!g_ptd3_status) { //if it was zero
		switch (d_next_state){
		case s1:
			PTB->PTOR=MASK(RED_LED); 	//toggle (from off to on)
			d_next_state=s2;
			break;
		case s2:
			delay(duration);
			d_next_state=s3;
			break;
		case s3:
			PTB->PTOR=MASK(RED_LED);	//toggle (on to off)
			d_next_state=s4;
			break;
		case s4:
			delay(duration);
			d_next_state=s5;
			break;
		case s5:
			PTD->PCOR|=MASK(BLUE_LED); // turn on (active low)
			d_next_state=s6;
			break;
		case s6:
			delay(duration);
			d_next_state=s7;
			break;
		case s7:
			PTD->PSOR|=MASK(BLUE_LED);  //turn off
			d_next_state=s8;
			break;
		case s8:
			delay(duration);
			d_next_state=s9;
			break;
		case s9:
			PTD->PSOR|=MASK(EXT_LED);	//turn on (active high)
			d_next_state=s10;
			break;
		case s10:
			delay(duration);
			d_next_state=s11;
			break;
		case s11:
			PTD->PCOR|=MASK(EXT_LED);	//turn off
			delay(duration);
			d_next_state=s1;
			break;
		default: d_next_state=s1; break;
		}
	}
}


#endif



//20971520u
#ifdef HALF_SEC

#include "MKL25Z4.h"   ///---------
#define MASK(x)			(1UL << x)

int main (void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    PORTB->PCR[18] = PORT_PCR_MUX(1) ;
    PTB->PDDR |= MASK(18);


    SysTick->LOAD = (20971520u/2u)-1 ;  //configure for every half sec restart.
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;   //enable+use system clock

    while (1)
    {
    // PTB->PTOR = MASK(18);
    //	delay(2);
        if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
        	PTB->PTOR = MASK(18);

    }
}
#endif


#ifdef MILLI_SEC

#include "MKL25Z4.h"   ///---------
#define MASK(x)			(1UL << x)


int main (void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    PORTB->PCR[18] = PORT_PCR_MUX(1) ;
    PTB->PDDR |= MASK(18);


    SysTick->LOAD = (20971520u/1000u)-1 ;  //configure for every millisecond restart.
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk; // 5;     /* enable it, no interrupt, use system clock */
unsigned int ctr=0;
    while (1)
    {
        if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
        	//PTB->PTOR = MASK(18);
        	ctr++;
			if(ctr==999){
				PTB->PTOR = MASK(18);
			ctr=0;
			}
        }
    }
}
#endif

//#define CHECK_CLOCK_SPEED
#ifdef CHECK_CLOCK_SPEED
int main (void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;// 0x0400;       /* enable clock to Port B */
    PORTB->PCR[18] = PORT_PCR_MUX(1) ; //0x100;     /* make PTB18 pin as GPIO */
    PTB->PDDR |= MASK(18); // 0x040000;      /* make PTB18 as output pin */

    PORTD->PCR[EXT] = PORT_PCR_MUX(1) ;
    PTD->PDDR |= MASK(EXT);

    /* Configure SysTick */
    SysTick->LOAD = 20971520u/2u; //half a second.
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk; // 5;     /* enable it, no interrupt, use system clock */
//unsigned int ctr=0;
    while (1)
    {
        if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) { // 0x10000  /* if COUNT flag is set */
        	PTB->PTOR = MASK(18);
        	PTD->PTOR = MASK(EXT);
        }
//        	ctr++;
//			if(ctr==999){
//				PTB->PTOR = MASK(18); //0x040000;       /* toggle red LED */
//			ctr=0;
//			}
    }
}
#endif


#ifdef NON_CMSIS

#include "MKL25Z4.h"   ///---------
#define MASK(x)			(1UL << x)


int main (void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;// 0x0400;       /* enable clock to Port B */
    PORTB->PCR[18] = PORT_PCR_MUX(1) ; //0x100;     /* make PTB18 pin as GPIO */
    PTB->PDDR |= MASK(18); // 0x040000;      /* make PTB18 as output pin */

    /* Configure SysTick */
    SYST_RVR== 8388000 - 1;
    SYST_CSR |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

    while (1)
    {
    	 if (SYST_CSR & SysTick_CTRL_COUNTFLAG_Msk)  // 0x10000  /* if COUNT flag is set */
            PTB->PTOR = MASK(18); //0x040000;       /* toggle red LED */
    }
}
#endif


 int i;
void delay(int delay)
{
   SysTick->LOAD = (20971520u/1000u)-1;  //roll over every millisecond.
   SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk; //this is 5


  
   for (i=0; i<delay; i++){
	   unsigned int ctr=0;
	   while (1){
		   if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
				ctr++; //count the milliseconds
				if(ctr==999){
					ctr=0;//one second has passed.
					break;
				}
		   }
	  }
   }
   SysTick->CTRL  = 0; //Stop the timer
}
//   while((SysTick->CTRL &0x10000) == 0) /*wait until the Count flag is set */
//   { }
//   SysTick->CTRL  = 0; /*Stop the timer (Enable = 0) */
//}



#ifdef interrupt_version

#include "MKL25Z4.h"   ///---------
#define MASK(x)			(1UL << x)


int main (void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    PORTB->PCR[18] = PORT_PCR_MUX(1) ;
    PTB->PDDR |= MASK(18);


    SysTick->LOAD = (20971520u/2u)-1 ;  //configure for every half sec restart.
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk |
    		SysTick_CTRL_ENABLE_Msk |SysTick_CTRL_TICKINT_Msk; //7 enable+use system clock +interrupts

    while (1)
    {  }//nothing to do. Just wait. interrupt handler will get called.
}

void SysTick_Handler(){
	PTB->PTOR = MASK(18);
}
#endif

#ifdef millis_version
volatile uint32_t counter=0;
unsigned long  millis();

int main (void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    PORTB->PCR[18] = PORT_PCR_MUX(1) ;
    PTB->PDDR |= MASK(18);


    SysTick->LOAD = (20971520u/1000u)-1 ;  //configure for every half sec restart.
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk |
    		SysTick_CTRL_ENABLE_Msk |SysTick_CTRL_TICKINT_Msk;

    unsigned long current_time =0u;
    unsigned long last_run=0u;
    unsigned long interval=1000; //1 sec

    while (1)
    {
    	//
    	current_time= millis();	//current time
		if((current_time-last_run) >= interval){
			last_run=current_time;
			PTB->PTOR = MASK(18);
		}
    }
}

void SysTick_Handler(){ 
	counter++;
}

unsigned long  millis(){
	return (unsigned long)counter;
}


#endif


