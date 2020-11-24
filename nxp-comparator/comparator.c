//no interrupts needed. Configure PTE0 to reflect CMP output
//external  PTE0 comes on/beeps  when CMP output is high
#include <mkl25z4.h>

#define PTE0		(0)		//PTE0, the CMP controlled LED
#define MASK(X)  (1<<X)

void Init_Comparator(void) ;
void init_CMP_pin(void);

int main(){
	init_CMP_pin();
	Init_Comparator();
	while(1);
}


void Init_Comparator(void) {
	SIM->SCGC4 |= SIM_SCGC4_CMP_MASK; 	// Clock gate comparator
	CMP0->CR1 = CMP_CR1_EN_MASK | CMP_CR1_OPE_MASK; // Enable CMP, output pin enable, PTE0
	//CMP0->CR1 |= CMP_CR1_INV_MASK ;		//invert the output
	//Plus select (Channel 4 PTE29), Minus Select (Channel 7- CMP DAC),
	CMP0->MUXCR = CMP_MUXCR_PSEL(5) | CMP_MUXCR_MSEL(7);
		// Enable DAC, reference voltage at 32/64 = 1/2 V; (use VOSEL =20 for 1V.)
	CMP0->DACCR = CMP_DACCR_DACEN_MASK | CMP_DACCR_VOSEL(32);
}


void init_CMP_pin(void){
	SIM->SCGC5 |=SIM_SCGC5_PORTE_MASK;
	PORTE->PCR[PTE0] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[PTE0] |= PORT_PCR_MUX(5);	//associate with the CMP.
	//PIN will go true high if PLUS is higher than MINUS
}