
//#define BASIC			//1 -by polling
#define TMP_INTERRUPT	//2 -by interrupt
//#define TMP_INTERRUPT2	//2 -by interrupt
//#define ALT_CLOCK		//3x	-by interrupt
//#define TPM2_CH0_OUTPUT_COMPARE	//4

//#define PWM			//5

#ifdef BASIC
#include <mkl25z4.h>
void init_LED();
void init_Timer();

#define RED_LED		(18)  	//B18
#define MASK(X)  (1<<X)
int main(){
	init_LED();
	init_Timer();
	while(1){
		while ((TPM0->SC & TPM_SC_TOF_MASK)==0 ){} //wait till count finished
		TPM0->SC |= TPM_SC_TOF_MASK;	//clear the flag
		PTB->PTOR |= MASK(RED_LED);
	}
}

void init_LED(){
	SIM->SCGC5 |=SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[RED_LED ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTB->PCR[RED_LED ] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PTB->PDDR |= MASK(RED_LED ) ;
	PTB->PSOR =MASK(RED_LED);	//turn off RED
}

void init_Timer(){
	//Clock gate
	SIM->SCGC6 |=SIM_SCGC6_TPM0_MASK;
	//Select clock source in SIM_SOPT:
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);	//1- MCGPLLCLK/2, or MCGFLLCLK 01 (ext?), 2-ext OSCERCLK, 3-internal MCGIRCLK
	//Configure registers
	TPM0->MOD= 0xFFFF;
	TPM0->SC |= TPM_SC_TOF_MASK; //Clear TOF before start,
	TPM0->SC |= TPM_SC_PS(7);	//optional pre-scale by 2^7 =128
	//Let selected timer (internal/external) run free
	TPM0->SC |= TPM_SC_CMOD(1); //the 00 LPTM disabled,	1-inc on each clock, 2 EXT clock
}

#endif



#ifdef TMP_INTERRUPT
#include <mkl25z4.h>
void init_LED();
void init_Timer();

#define RED_LED		(18)  	//B18
#define MASK(X)  (1<<X)
int main(){
	init_LED();
	init_Timer();

	while(1){}
}

void init_LED(){
	SIM->SCGC5 |=SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[RED_LED ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTB->PCR[RED_LED ] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PTB->PDDR |= MASK(RED_LED ) ;
	PTB->PSOR =MASK(RED_LED);	//turn off RED
}

void init_Timer(){
	//Clock gate
	SIM->SCGC6 |=SIM_SCGC6_TPM0_MASK;
	//Select clock source in SIM_SOPT
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);	//1- MCGPLLCLK/2, or MCGFLLCLK 01 (ext?), 2-ext OSCERCLK, 3-internal MCGIRCLK
	//Configure registers
	TPM0->MOD= 0xFFFF;

	TPM0->SC |= TPM_SC_TOF_MASK; //Clear TOF before start,
	TPM0->SC |= TPM_SC_PS(7);	//optional pre-scale by 2^7 =128
	TPM0->SC |= TPM_SC_TOIE_MASK; //Enable TOIE for interrupt
	TPM0->SC |= TPM_SC_CMOD(1); //enable internal clock to run

	NVIC_ClearPendingIRQ(TPM0_IRQn);
	NVIC_SetPriority(TPM0_IRQn, 3);
	NVIC_EnableIRQ(TPM0_IRQn);
	//__enable_irq();
}
//TPM0->SC |=  TPM_SC_TOF_MASK | TPM_SC_PS(7) | TPM_SC_TOIE_MASK  |TPM_SC_CMOD(1);
//ps(5) is about 100ms


void TPM0_IRQHandler(){
	PTB->PTOR |= MASK(RED_LED);
	TPM0->SC |= TPM_SC_TOF_MASK ; //clear the interrupt
}

#endif



#ifdef TMP_INTERRUPT2
#include <mkl25z4.h>
void init_LED();
void init_Timer();

#define RED_LED		(18)  	//B18
#define MASK(X)  (1<<X)
int main(){
	init_LED();
	init_Timer();

	while(1){}
}

void init_LED(){
	SIM->SCGC5 |=SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[RED_LED ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTB->PCR[RED_LED ] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PTB->PDDR |= MASK(RED_LED ) ;
	PTB->PSOR =MASK(RED_LED);	//turn off RED
}

void init_Timer(){
	//Clock gate
	SIM->SCGC6 |=SIM_SCGC6_TPM0_MASK;
	//Select clock source in SIM_SOPT
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);	//1- MCGPLLCLK/2, or MCGFLLCLK 01 (ext?), 2-ext OSCERCLK, 3-internal MCGIRCLK
	//Configure registers
	TPM0->MOD= 0xFFFF;

	TPM0->SC |= TPM_SC_TOF_MASK; //Clear TOF before start,
	TPM0->SC |= TPM_SC_PS(5);	//optional pre-scale by 2^7 =128
	TPM0->SC |= TPM_SC_TOIE_MASK; //Enable TOIE for interrupt
	TPM0->SC |= TPM_SC_CMOD(1); //enable internal clock to run

	NVIC_ClearPendingIRQ(TPM0_IRQn);
	NVIC_SetPriority(TPM0_IRQn, 3);
	NVIC_EnableIRQ(TPM0_IRQn);
}
//TPM0->SC |=  TPM_SC_TOF_MASK | TPM_SC_PS(7) | TPM_SC_TOIE_MASK  |TPM_SC_CMOD(1);
//ps(5) is about 100ms


void TPM0_IRQHandler(){
	static int ctr=1;
	ctr++;
	if (ctr>=10){
		PTB->PTOR |= MASK(RED_LED);
		ctr=0;
	}
	TPM0->SC |= TPM_SC_TOF_MASK ; //clear the interrupt
}

#endif



#ifdef ALT_CLOCK
#include <mkl25z4.h>
void init_LED();
void init_Timer();

#define RED_LED		(18)  	//B18
#define MASK(X)  (1<<X)
int main(){
	init_LED();
	init_Timer();

	while(1){}
}

void init_LED(){
	SIM->SCGC5 |=SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[RED_LED ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTB->PCR[RED_LED ] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PTB->PDDR |= MASK(RED_LED ) ;
	PTB->PSOR =MASK(RED_LED);	//turn off RED
}

void init_Timer(){
//	MCG->C1
	//Clock gate
	SIM->SCGC6 |=SIM_SCGC6_TPM0_MASK;
	//Select clock source in SIM_SOPT
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(3);	//3-internal MCGIRCLK
	//Configure registers
	TPM0->MOD= 0xFFFF;

	TPM0->SC |= TPM_SC_TOF_MASK; //Clear TOF before start,
	TPM0->SC |= TPM_SC_PS(7);	//optional pre-scale by 2^7 =128
	TPM0->SC |= TPM_SC_TOIE_MASK; //Enable TOIE for interrupt
	TPM0->SC |= TPM_SC_CMOD(1); //enable internal clock to run

	NVIC_ClearPendingIRQ(TPM0_IRQn);
	NVIC_SetPriority(TPM0_IRQn, 3);
	NVIC_EnableIRQ(TPM0_IRQn);
}
//TPM0->SC |=  TPM_SC_TOF_MASK | TPM_SC_PS(7) | TPM_SC_TOIE_MASK  |TPM_SC_CMOD(1);
//ps(5) is about 100ms


void TPM0_IRQHandler(){
	PTB->PTOR |= MASK(RED_LED);
	TPM0->SC |= TPM_SC_TOF_MASK ; //clear the interrupt
}

#endif




#ifdef TPM2_CH0_OUTPUT_COMPARE2
//stop and restart, also uses PTD3 output
//
#include <mkl25z4.h>
void init_LED();
void init_Timer();
void init_pin();

#define RED_LED		(18)  	//B18 TPM2_CH0 is ALT3 see p163 of spec
#define EXT1		(3)		//PTD3
#define MASK(X)  (1<<X)

int main(){
	init_LED();
	init_Timer();
	init_pin();

	while(1){}
}

void init_LED(){

	SIM->SCGC5 |=SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[RED_LED ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTB->PCR[RED_LED ] |= PORT_PCR_MUX(3);	//***setup to be output of TPM****

	//no longer needed! not GPIO but timer controlled
	//PTB->PDDR |= MASK(RED_LED ) ;
	//PTB->PSOR =MASK(RED_LED);	//turn off RED

}

void init_Timer(){
	//Clock gate
	SIM->SCGC6 |=SIM_SCGC6_TPM2_MASK;	//*******TPM2 channel 0
	//Select clock source in SIM_SOPT
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);	//1- MCGPLLCLK/2, or MCGFLLCLK 01 (ext?), 2-ext OSCERCLK, 3-internal MCGIRCLK
	//Configure registers
	TPM2->MOD= 0xFFFF;

	//working wth TPM2_C0SC
						//output compare + TOGGLE on match
	TPM2->CONTROLS[0].CnSC |= TPM_CnSC_MSA(1) | TPM_CnSC_ELSA(1) ;
	//next line is for output compare + CLEAR on match (because it is for onboard LED, it comes on when matched
	//TPM2->CONTROLS[0].CnSC |= TPM_CnSC_MSA(1) | TPM_CnSC_ELSB(1) | TPM_CnSC_ELSA(0) ; //clear on mathc 10

	TPM2->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK;  //clear spurious interrupts
	TPM2->CONTROLS[0].CnV =0x7FFF;	//no atter the time, the period will be same just delayed/phase shift
	TPM2->SC |=  TPM_SC_TOF_MASK | TPM_SC_PS(7) | TPM_SC_TOIE_MASK  ;

	TPM2->SC |= TPM_SC_CMOD(1); //enable internal clock to run

	NVIC_ClearPendingIRQ(TPM2_IRQn);
	NVIC_SetPriority(TPM2_IRQn, 3);
	NVIC_EnableIRQ(TPM2_IRQn);
}
//TPM0->SC |=  TPM_SC_TOF_MASK | TPM_SC_PS(7) | TPM_SC_TOIE_MASK  |TPM_SC_CMOD(1);
//ps(5) is about 100ms


void TPM2_IRQHandler(){
	//optionally check the flag
	if (TPM2->STATUS & TPM_STATUS_CH0F_MASK){		//alternative check using the status flag
	//if (TPM2->CONTROLS[0].CnSC & TPM_CnSC_CHF_MASK){
		TPM2->CONTROLS[0].CnSC |=TPM_CnSC_CHF_MASK;//clear flag
	}
	PTD->PTOR |= MASK(EXT1 ) ; //THis is for debugging purposes. not needed.
	TPM2->SC |= TPM_SC_TOF_MASK ; //clear the interrupt
}


void init_pin(){
	SIM->SCGC5 |=SIM_SCGC5_PORTD_MASK;
	PORTD->PCR[EXT1] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[EXT1] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PTD->PDDR |= MASK(EXT1 ) ;
	PTD->PCOR |= MASK(EXT1 ) ; //set to 0
}

#endif

#ifdef INPUT_CAPTURE		//count signal
#endif

#ifdef COUNT				//count input pulses
#endif

#ifdef PWM					//generate PWM
#include <mkl25z4.h>
void init_LED();
void init_Timer();
void init_pin();

#define RED_LED		(18)  	//B18 TPM2_CH0 is ALT3 see p163 of spec
#define EXT1		(3)		//PTD3
#define MASK(X)  (1<<X)
#define PTB2		(2)		//PTB2 to monitor on scope

int main(){
	init_LED();
	init_Timer();
	init_pin();

	while(1){}
}

void init_LED(){

	SIM->SCGC5 |=SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[RED_LED ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTB->PCR[RED_LED ] |= PORT_PCR_MUX(3);	//***setup to be output of TPM2_CH0****

	PORTB->PCR[PTB2 ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTB->PCR[PTB2] |= PORT_PCR_MUX(3);	//***setup to be output of TPM2_CH0****


}

void init_Timer(){
	//Clock gate
	SIM->SCGC6 |=SIM_SCGC6_TPM2_MASK;	//*******TPM2 channel 0
	//Select clock source in SIM_SOPT
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);	//1- MCGPLLCLK/2, or MCGFLLCLK 01 (ext?), 2-ext OSCERCLK, 3-internal MCGIRCLK
	//Configure registers
	TPM2->MOD= 0xFFFF;

	//working wth TPM2_C0SC
						//output compare + edge aligned PWM MSBA: 10, ELSBA:10
	TPM2->CONTROLS[0].CnSC |= TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1) ;
	TPM2->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK;  //clear spurious interrupts
	TPM2->CONTROLS[0].CnV =0x7FFF;	//no atter the time, the period will be same just delayed/phase shift
	TPM2->SC |=  TPM_SC_TOF_MASK | TPM_SC_PS(7) | TPM_SC_TOIE_MASK  ;

	TPM2->SC |= TPM_SC_CMOD(1); //enable internal clock to run

	NVIC_ClearPendingIRQ(TPM2_IRQn);
	NVIC_SetPriority(TPM2_IRQn, 3);
	NVIC_EnableIRQ(TPM2_IRQn);
}
//TPM0->SC |=  TPM_SC_TOF_MASK | TPM_SC_PS(7) | TPM_SC_TOIE_MASK  |TPM_SC_CMOD(1);
//ps(5) is about 100ms


void TPM2_IRQHandler(){
	//optionally check the flag
	if (TPM2->STATUS & TPM_STATUS_CH0F_MASK){		//alternative check using the status flag
	//if (TPM2->CONTROLS[0].CnSC & TPM_CnSC_CHF_MASK){
		TPM2->CONTROLS[0].CnSC |=TPM_CnSC_CHF_MASK;//clear flag
	}
	PTD->PTOR |= MASK(EXT1 ) ; //THis is for debugging purposes. not needed.
	TPM2->SC |= TPM_SC_TOF_MASK ; //clear the interrupt
}


void init_pin(){
	SIM->SCGC5 |=SIM_SCGC5_PORTD_MASK;
	PORTD->PCR[EXT1] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[EXT1] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PTD->PDDR |= MASK(EXT1 ) ;
	PTD->PCOR |= MASK(EXT1 ) ; //set to 0
}

#endif

