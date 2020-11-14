#include "mkl25z4.h"


//defining pins
#define RS (2)  //PTA2
#define E  (1)  //PTA1
#define DB0 (4) //PTD4
#define DB1 (12) //PTA12
#define DB2 (4)   //PTA4
#define DB3 (5)   //PTA5
#define DB4 (8)   //PTC8
#define DB5 (9)   //PTC9
#define DB6 (13)   //PTA13
#define DB7  (5) //PTD5

#define MASK(x)			(1UL << x)

void init_pins(){
	//clock gating to port B & D
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK ; //same as above

	//set up 2 pins as GPIO --> configure mux to connect pins as GPIO
	PORTA->PCR[RS ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTA->PCR[RS ] |= PORT_PCR_MUX(1);	//setup to be GPIO
	
	PORTA->PCR[E] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTA->PCR[E] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT
	
	PORTD->PCR[DB0] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[DB0] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT

	PORTA->PCR[DB1 ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTA->PCR[DB1 ] |= PORT_PCR_MUX(1);	//setup to be GPIO

	PORTA->PCR[DB2 ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTA->PCR[DB2 ] |= PORT_PCR_MUX(1);	//setup to be GPIO
	
	PORTA->PCR[DB3 ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTA->PCR[DB3 ] |= PORT_PCR_MUX(1);	//setup to be GPIO
	
	PORTC->PCR[DB4] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTC->PCR[DB4] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT
	
	PORTC->PCR[DB5] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTC->PCR[DB5] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT
	
	PORTA->PCR[DB6 ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTA->PCR[DB6 ] |= PORT_PCR_MUX(1);	//setup to be GPIO
	
	PORTD->PCR[DB7] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[DB7] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT	
	
	//setting up pins to be output
	PTA->PDDR |= MASK(RS) ;
	PTA->PDDR |= MASK(E) ;
	PTD->PDDR |= MASK(DB0) ;
	PTA->PDDR |= MASK(DB1) ;
	PTA->PDDR |= MASK(DB2) ;
	PTA->PDDR |= MASK(DB3) ;
	PTC->PDDR |= MASK(DB4) ;
	PTC->PDDR |= MASK(DB5) ;
	PTA->PDDR |= MASK(DB6) ;
	PTD->PDDR |= MASK(DB7) ;

}

void delay_ms(int t_ms){
	int i,j;
	for (i=0; i<t_ms; i++)
		for (j=0; j<48000;j++);
}

static void write_to_data_pins(uint8_t data){
        uint32_t gpio_temp;
     
       // Get GPIO-C Data Register
       gpio_temp = GPIOC->PDOR;
     
      // Update the LCD data line DB7
       if (data & 0x80){
            gpio_temp |= DB7;
        }
        else{
            gpio_temp &= ~DB7;
        }
         
        // Update the LCD data line DB6
        if (data & 0x40){
            gpio_temp |= DB6;
        }
        else{
            gpio_temp &= ~DB6;
        }
         
        // Update the LCD data line DB5
        if (data & 0x20){
            gpio_temp |= DB5;
        }
        else{
            gpio_temp &= ~DB5;
        }
         
        // Update the LCD data line DB4
        if (data & 0x10){
            gpio_temp |= DB4;
        }
        else{
            gpio_temp &= ~DB4;
        }
         
        // Write updated data to GPIO-C Port
        GPIOC->PDOR = gpio_temp;
}
 

void lcd_write_instruc (unsigned char instruction){
	delay_ms(2);
//set RS pin LOW, it is an instruction not data
	PTA->PCOR|=MASK(RS);
//set E LOW,
	PTA->PCOR|=MASK(E);
	delay_ms(1);
//write instruction
	write_to_data_pins((instruction << 4) & 0xF0);
//set E  HIGH, then LOW, (generate falling edge to write)
	PTA->PSOR|=MASK(E);
	delay_ms(1);
	PTA->PCOR|=MASK(E);
	}

void lcd_write_char (unsigned char c){
delay_ms(2);
	//set RS pin HIGH, it is data
	PTA->PSOR|=MASK(RS);
	//set E LOW,
	PTA->PCOR|=MASK(E);
	delay_ms(1);
//write data
	write_to_data_pins((c << 4) & 0xF0);
	//set E  HIGH, then LOW, (generate falling edge to write)
	PTA->PSOR|=MASK(E);
	delay_ms(1);
	PTA->PCOR|=MASK(E);
	delay_ms(1);
	
	}


	void lcd_init (void)
	{
    	delay_ms(2);
    	lcd_write_instruc(0x06);        //Increment mode for the cursor
    	lcd_write_instruc(0x0C);        //The display on, the cursor off
    	lcd_write_instruc(0x38);        //An 8-bit data bus, two-line display
	}


void lcd_clear(void)
	{
	delay_ms(2);
	lcd_write_instruc(0x01);        //Clear the display
	lcd_write_instruc(0x02);        //returns home, even if shifted/but not clear display
}


void lcd_goto(unsigned char column, unsigned char row)
	{
	delay_ms(2);
	if(row==0)
	lcd_write_instruc(0x80 + column);
	if(row==1)
	lcd_write_instruc(0xC0+ column);
	}

void lcd_write_string(char *s)
	{
	delay_ms(2);
	while(*s != 0)
	{
	lcd_write_char(*s);
	s++;
	}
	}

	///////////////////////////////////////////////////////////////

int main(void)
{

	init_pins();
	
	lcd_init();
	lcd_clear();
	
	lcd_goto(0,0);
	lcd_write_string("Ashesi Embedded");
	lcd_goto(0,1);
	lcd_write_string(" Embedded Sys ");
	
while (1)
    {
        continue;
    }
}