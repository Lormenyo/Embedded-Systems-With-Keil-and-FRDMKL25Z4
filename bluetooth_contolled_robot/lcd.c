#include "lcd.h"


void init_lcd_pins(){
	//clock gating to port B & D
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTB_MASK; //same as above

	//set up 2 pins as GPIO --> configure mux to connect pins as GPIO
	PORTC->PCR[RS] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTC->PCR[RS] |= PORT_PCR_MUX(1);	//setup to be GPIO
	
	PORTC->PCR[E] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTC->PCR[E] |= PORT_PCR_MUX(1);	//setup to be GPIO, this takes actual ALT
	
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
	PTC->PDDR |= MASK(RS) ;
	PTC->PDDR |= MASK(E) ;
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


static void write_to_data_pins(unsigned char data){
     
      // Update the LCD data line DB7
       if (data & MASK(7)){
            PTD->PSOR |= MASK(DB7);
        }
        else{
            PTD->PCOR |= MASK(DB7);
        }
         
        // Update the LCD data line DB6
        if (data & MASK(6)){
            PTA->PSOR |= MASK(DB6);
        }
        else{
            PTA->PCOR |= MASK(DB6);
        }
         
        // Update the LCD data line DB5
        if (data & MASK(5)){
            PTC->PSOR |= MASK(DB5);
        }
        else{
            PTC->PCOR |= MASK(DB5);
        }
         
        // Update the LCD data line DB4
        if (data & MASK(4)){
            PTC->PSOR |= MASK(DB4);
        }
        else{
            PTC->PCOR |= MASK(DB4);
        }
				
				// Update the LCD data line DB3
        if (data & MASK(3)){
            PTA->PSOR |= MASK(DB3);
        }
        else{
            PTA->PCOR |= MASK(DB3);
        }
 
				// Update the LCD data line DB2
        if (data & MASK(2)){
            PTA->PSOR |= MASK(DB2);
        }
        else{
            PTA->PCOR |= MASK(DB2);
        } 

				// Update the LCD data line DB1
				if (data & MASK(1)){
            PTA->PSOR |= MASK(DB1);
        }
        else{
            PTA->PCOR |= MASK(DB1);
        }
				
				// Update the LCD data line DB0
				if (data & MASK(0)){
            PTD->PSOR |= MASK(DB0);
        }
        else{
            PTD->PCOR |= MASK(DB0);
        }
}
 

void lcd_write_instruc (unsigned char instruction){
	delay_ms(2);
	//set RS pin LOW, it is an instruction not data
	PTC->PCOR|=MASK(RS);
	//set E LOW
	PTC->PCOR|=MASK(E);
	
	//write instruction
	write_to_data_pins(instruction);
	//set E  HIGH, then LOW, (generate falling edge to write)
	PTC->PSOR|=MASK(E);
	PTC->PCOR|=MASK(E);
}


void lcd_write_char (unsigned char c){
	delay_ms(2);
	//set RS pin HIGH, it is data
	PTC->PSOR|=MASK(RS);
	//set E LOW,
	PTC->PCOR|=MASK(E);

	//write data
	write_to_data_pins(c);
	//set E  HIGH, then LOW, (generate falling edge to write)
	PTC->PSOR|=MASK(E);
	PTC->PCOR|=MASK(E);

}


void lcd_init (void){
    	delay_ms(2);
    	lcd_write_instruc(CURSOR_INCREMENT_MODE_INSTRUCTION);        //Increment mode for the cursor
    	lcd_write_instruc(DISPLAY_ON_INSTRUCTION);        //The display on, the cursor off
    	lcd_write_instruc(TWO_LINE_DISPLAY_INSTRUCTION);        //An 8-bit data bus, two-line display
}


void lcd_clear(void){
	delay_ms(2);
	lcd_write_instruc(CLEAR_DISPLAY_INSTRUCTION);        //Clear the display
	lcd_write_instruc(RETURN_HOME_INSTRUCTION);        //returns home, even if shifted/but not clear display
}


void lcd_goto(unsigned char column, unsigned char row){
	delay_ms(2);  
	if(row==0)
	lcd_write_instruc(FIRST_ROW + column);
	if(row==1)
	lcd_write_instruc(SECOND_ROW + column);
}


void lcd_write_string(char *s){
	delay_ms(2);
	while(*s != 0){
	lcd_write_char(*s);
	s++;
	}
}

void lcd_scroll_right(unsigned char row_number,char* text){
	//scrolls just a row
	int i;
	static int column_position=0;
	unsigned char row_full_length = 16;
	for(i=0; column_position <= row_full_length; i++){
		column_position++;
		lcd_clear();
		lcd_goto(column_position, row_number);
		lcd_write_string(text);
//		delay_ms(1);
	}
}


void lcd_scroll_screen(unsigned char row_number,char* text){
	int i, column_position;
	
	unsigned char row_full_length = 16;
	if (strlen(text) < 16){
		column_position=0;
		lcd_write_string(text);
		for(i=0; column_position <= row_full_length; i++){
			column_position++;
			lcd_write_instruc(SHIFT_RIGHT);
			delay_ms(50);
	}
	}
		else{
			
			for(i=0; text[i] != '\0'; i++){				
				lcd_write_char(text[i]);
				lcd_write_instruc(SHIFT_LEFT);
				delay_ms(20);
		}
}
}
