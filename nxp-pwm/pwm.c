#include <mkl25z4.h>

// 20ms (50Hz), with pulse widths of 1ms, 1.5 ms and 2ms


void init_output_pins();
void init_Timer();
void init_pin();

#define PTC1		(1)  	//B18 TPM0_CH0 is ALT3 see p163 of spec
#define PTA4		(4)  	//B18 TPM0_CH1 is ALT3 see p163 of spec
#define PTA5		(5)  	//B18 TPM0_CH2 is ALT3 see p163 of spec
#define EXT1		(3)		//PTD3
#define MASK(X)  (1<<X)
#define PTB2		(2)		//PTB2 to monitor on scope


int main(){
	init_output_pins();
	init_Timer();
	init_pin();

	while(1){}
}

void init_output_pins(){
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTC_MASK;
	PORTC->PCR[PTC1 ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTC->PCR[PTC1 ] |= PORT_PCR_MUX(4);	//***setup to be output of TPM0_CH0****

	PORTA->PCR[PTA4 ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTA->PCR[PTA4 ] |= PORT_PCR_MUX(3);	//***setup to be output of TPM0_CH0****
	
	PORTA->PCR[PTA5 ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTA->PCR[PTA5 ] |= PORT_PCR_MUX(3);	//***setup to be output of TPM0_CH0****
	
	PORTB->PCR[PTB2 ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTB->PCR[PTB2] |= PORT_PCR_MUX(3);	//
}


void init_Timer(){
	//Clock gate
	SIM->SCGC6 |=SIM_SCGC6_TPM0_MASK;	//*******TPM0 channel 0
	//Select clock source in SIM_SOPT
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);	//1- MCGPLLCLK/2, or MCGFLLCLK 01 (ext?), 2-ext OSCERCLK, 3-internal MCGIRCLK
	//Configure registers
	TPM0->MOD= 3277u;

	//working wth TPM2_C0SC
	// edge aligned PWM MSBA: 10, ELSBA:10 - true high
	TPM0->CONTROLS[0].CnSC |= TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1) ;
	TPM0->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK;  //clear spurious interrupts
	TPM0->CONTROLS[0].CnV = 164u;	//no atter the time, the period will be same just delayed/phase shift
	
	TPM0->CONTROLS[1].CnSC |= TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1) ;
	TPM0->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK;  //clear spurious interrupts
	TPM0->CONTROLS[1].CnV = 246u;	//no atter the time, the period will be same just delayed/phase shift
	
	TPM0->CONTROLS[2].CnSC |= TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1) ;
	TPM0->CONTROLS[2].CnSC |= TPM_CnSC_CHF_MASK;  //clear spurious interrupts
	TPM0->CONTROLS[2].CnV = 328u;	//no atter the time, the period will be same just delayed/phase shift
	
	TPM0->SC |=  TPM_SC_TOF_MASK | TPM_SC_PS(7) | TPM_SC_TOIE_MASK  ;

	TPM0->SC |= TPM_SC_CMOD(1); //enable internal clock to run

	NVIC_ClearPendingIRQ(TPM0_IRQn);
	NVIC_SetPriority(TPM0_IRQn, 3);
	NVIC_EnableIRQ(TPM0_IRQn);
}


void TPM0_IRQHandler(){
	//optionally check the flag
	if (TPM0->STATUS & TPM_STATUS_CH0F_MASK){		//alternative check using the status flag
	//if (TPM2->CONTROLS[0].CnSC & TPM_CnSC_CHF_MASK){
		TPM0->CONTROLS[0].CnSC |=TPM_CnSC_CHF_MASK;//clear flag
	}
	
	if (TPM0->STATUS & TPM_STATUS_CH1F_MASK){		//alternative check using the status flag
	//if (TPM2->CONTROLS[0].CnSC & TPM_CnSC_CHF_MASK){
		TPM0->CONTROLS[1].CnSC |=TPM_CnSC_CHF_MASK;//clear flag
	}
		
		if (TPM0->STATUS & TPM_STATUS_CH2F_MASK){		//alternative check using the status flag
	//if (TPM2->CONTROLS[0].CnSC & TPM_CnSC_CHF_MASK){
		TPM0->CONTROLS[2].CnSC |=TPM_CnSC_CHF_MASK;//clear flag
	}
		
	PTD->PTOR |= MASK(EXT1 ) ; //THis is for debugging purposes. not needed.
	TPM0->SC |= TPM_SC_TOF_MASK ; //clear the interrupt
}


void init_pin(){
	SIM->SCGC5 |=SIM_SCGC5_PORTD_MASK;
	PORTD->PCR[EXT1] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[EXT1] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PTD->PDDR |= MASK(EXT1 ) ;
	PTD->PCOR |= MASK(EXT1 ) ; //set to 0
}