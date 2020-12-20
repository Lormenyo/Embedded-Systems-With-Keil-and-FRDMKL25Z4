#define RX	1		//PTA1
#define TX	2		//PTA2
#define OSR 15		//over sample rate (like a pre-scaler)
#define BAUD_RATE  	9600	//my communication rate on BT
#define SYS_CLOCK	20971520u	//
#define RED_LED_POS (18)		// on port B
#define GREEN_LED_POS (19)	// on port B
#define BLUE_LED_POS (1)		// on port D

void init_bluetooth_pins(void);
void Init_UART(void);
void doTxRx_task(void);
void carBluetoothControl(void);




