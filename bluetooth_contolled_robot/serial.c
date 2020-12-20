#include "general.h"
#include "queue.h"
#include "serial.h"



volatile char rxChar;
uint8_t sbr;

Q_T TxQ, RxQ;

extern void stop_car(void);
extern void turn_car_left(void);
extern void turn_car_right(void);
extern void forward_motors(void);
extern void reverse_motors(void);
extern void slow_down(void);
extern void move_faster(void);

void init_bluetooth_pins(void){
	//Clock gate port A
	SIM->SCGC5 |= SIM_SCGC5_PORTA(1);
	PORTA->PCR[RX] &= ~PORT_PCR_MUX_MASK;  	//clear mux
	PORTA->PCR[RX] |=  PORT_PCR_MUX(2); 	//set for UART0 RX
	PORTA->PCR[TX] &= ~PORT_PCR_MUX_MASK;	//clear
	PORTA->PCR[TX] |=  PORT_PCR_MUX(2); 	//set for UART0 TX
}

void Init_UART(void){
	//select clock for uart0 (disabled by default), MCGFLLCLK/system clk as UART0 clock 
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
	// clock gate UART0
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;		//clock gate UART0
	//compute set baud rate (SBR), choosing baud rate of 9600 for BT
	sbr = (uint16_t)((SYS_CLOCK)/((OSR+1) *BAUD_RATE ));	//default OSR is 15, 	sbr=136.5 if SYS_CLOCK =20971520u
	//UART0->BDH |=((sbr>>8) & 0x1F);	//generic. set only bottom 5 bits
	UART0->BDH =0;			//0x0 for this calculation, other fields are default 0. 
	UART0->BDL=sbr;			//0x88 for this calculation
	// Rx Interrupt enabled, Tx & RX enable
	UART0->C2  |= UART_C2_RIE_MASK | UART_C2_TE_MASK | UART_C2_RE_MASK;
	//note: default is 8N1 if uart0->C1=0

	NVIC_SetPriority(UART0_IRQn, 3);
	NVIC_ClearPendingIRQ(UART0_IRQn);
	NVIC_EnableIRQ(UART0_IRQn);
}

void Init_UART_fuller(void){
	//select clock for uart0 (disabled by default), MCGFLLCLK as UART0 clock (system??)
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
	// clock gate UART0
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;		//clock gate UART0
	//set baud rate (SBR), choosing baud rate of 9600
	sbr = (uint16_t)((SYS_CLOCK)/((OSR+1) *BAUD_RATE ));	//default OSR is 15, sbr=136.5 if SYS_CLOCK =20971520u

	//UART0->C2 &= ~UART0_C2_TE_MASK & ~UART0_C2_RE_MASK; //disable TX and Rx before configuring
	//configure 8bits, no parity  **(1 stop bit) //default settings
	//UART0->C1  = 0x00;		//8 bits, no parity
	//UART0->BDH &= ~UART0_BDH_SBNS_MASK;		// 1Stop Bit Number Select (alt is 2)
	//set baud rate
	UART0->BDH=(sbr>>8);	//0;
	UART0->BDL=sbr;			//0x88 for this calculation


	// Configure Tx/Rx Interrupts
	//UART0->C2  &= ~UART_C2_TIE_MASK;  // Tx Interrupt disabled, can also do |=UART_C2_TIE(0);
	//UART0->C2  &= ~UART_C2_TCIE_MASK; // Tx Complete Interrupt disabled, can also do |=UART_C2_TCIE(0)

	// Rx Interrupt enabled, Tx & RX enable
	UART0->C2  |= UART_C2_RIE_MASK | UART_C2_TE_MASK | UART_C2_RE_MASK;


	NVIC_SetPriority(UART0_IRQn, 3);
	NVIC_ClearPendingIRQ(UART0_IRQn);
	NVIC_EnableIRQ(UART0_IRQn);
}



void carBluetoothControl(void){
	switch (rxChar){
		case 'f': forward_motors();;
				break;
		case 'b': reverse_motors();
				break;
		case 'l': turn_car_left();
				break;
		case 'r': turn_car_right();
				break;
		case 'w': slow_down();
				break;
		case 't': move_faster();
				break;
		default:
				stop_car();
	}
}




void UART0_IRQHandler(void) {
	uint8_t ch;

	if (UART0->S1 & (UART_S1_OR_MASK |UART_S1_NF_MASK |
		UART_S1_FE_MASK | UART_S1_PF_MASK)) {
			// clear the error flags
			UART0->S1 |= UART0_S1_OR_MASK | UART0_S1_NF_MASK |
									UART0_S1_FE_MASK | UART0_S1_PF_MASK;
			// read the data register to clear RDRF
			ch = UART0->D;
	}
	if (UART0->S1 & UART0_S1_RDRF_MASK) {
		// received a character
		ch = UART0->D;
		rxChar=ch;		//to enable me take some action
		carBluetoothControl();		//deal with input
		if (!Q_Full(&RxQ)) {
			Q_Enqueue(&RxQ, ch);
		} else {
			// error - queue full, discard character
		}
	}
	if ( (UART0->C2 & UART0_C2_TIE_MASK) && // transmitter interrupt enabled
			(UART0->S1 & UART0_S1_TDRE_MASK) ) { // tx buffer empty
		// can send another character
		if (!Q_Empty(&TxQ)) {
			UART0->D = Q_Dequeue(&TxQ);
		} else {
			// queue is empty so disable transmitter interrupt
			UART0->C2 &= ~UART0_C2_TIE_MASK;
		}
	}
}


void doTxRx_task(){
	uint8_t buffer[80], c, * bp;
	if (Q_Size(&RxQ) == 0)
		return;	//nothing received. Exit
	
	//echo character sent:
	c = Q_Dequeue(&RxQ);
	sprintf((char *) buffer, "Data sent %c ", c);
	// enqueue string
	bp = buffer;
	while (*bp != '\0') {	//enqueue full string
		while (Q_Full(&TxQ)) ; // wait if buffer full
		Q_Enqueue(&TxQ, *bp);
		bp++;
	}
	// start transmitter if it isn't already running
	if (!(UART0->C2 & UART0_C2_TIE_MASK)) {
		UART0->C2 |= UART0_C2_TIE(1);
	}

}
