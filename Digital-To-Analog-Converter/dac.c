
#include <mkl25z4.h>

#define PTE30 (30)
#define TOP4BITS(X)	((X>>8) & 0xF)
void init_DAC();
void init_pin();
void init_systick();

int main(){
	init_systick();
	init_pin();
	init_DAC();

	while(1){};
}


void init_DAC(void) {
  // clock gate
	SIM->SCGC6 |= SIM_SCGC6_DAC0_MASK;
	//default setting: disable buffer mode. (All control registers =0)
	// Enable DAC, select VDDA as reference voltage (Analog Power Supply)
	DAC0->C0 = DAC_C0_DACEN_MASK | DAC_C0_DACRFS_MASK;						//can even afford to use Vrefh rfs=0
}

void init_pin(){
	//set up pin PTE30 for DAC out
	SIM->SCGC5 |=SIM_SCGC5_PORTE_MASK;
	PORTE->PCR[PTE30] &=~PORT_PCR_MUX_MASK;
	PORTE->PCR[PTE30] |=PORT_PCR_MUX(0); //DAC out
}

void init_systick(){
	//initialize systick
	SysTick->LOAD = (20971520u/1000u)-1 ;  //. Use timebase 1s. Configure duration. demo effect.
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk |
				SysTick_CTRL_ENABLE_Msk |SysTick_CTRL_TICKINT_Msk;
}

void SysTick_Handler(){
	//sawtooth
	int delta = 2;		//demo effect
	static int val =0;		//val is the digital value to convert to analog
	val += delta;
	if (val >= 4095) {
		val=0;
	}
	DAC0->DAT[0].DATL = DAC_DATL_DATA0(val);
	DAC0->DAT[0].DATH = TOP4BITS(val);
}
