#include "mkl25z4.h"
#include <string.h>

//defining pins
#define RS (7)  //PTC7
#define E  (0)  //PTC0
#define DB0 (4) //PTD4
#define DB1 (12) //PTA12
#define DB2 (4)   //PTA4
#define DB3 (5)   //PTA5
#define DB4 (8)   //PTC8
#define DB5 (9)   //PTC9
#define DB6 (13)   //PTA13
#define DB7  (5) //PTD5



#define MASK(x)	(1UL << x)
#define FIRST_ROW (0x80)
#define SECOND_ROW (0xC0)
#define CURSOR_INCREMENT_MODE_INSTRUCTION (0x06)
#define DISPLAY_ON_INSTRUCTION (0x0C)
#define TWO_LINE_DISPLAY_INSTRUCTION (0x38)
#define CLEAR_DISPLAY_INSTRUCTION (0x01)
#define RETURN_HOME_INSTRUCTION (0x02)
#define SHIFT_RIGHT (0x1C)
#define SHIFT_LEFT (0x18)

void lcd_write_string(char *s);
void lcd_goto(unsigned char column, unsigned char row);
void lcd_init (void);
void lcd_write_char (unsigned char c);
void lcd_write_instruc (unsigned char instruction);
static void write_to_data_pins(unsigned char data);
void delay_ms(int t_ms);
void init_lcd_pins(void);
void lcd_write_string(char *s);
void lcd_clear(void);
void lcd_scroll_right(unsigned char row_number,char* text);
void lcd_scroll_screen(unsigned char row_number,char* text);

///////////////////////////////////////////////////////////////
//unsigned char text[] = {"Lab Assignment for displaying text on LCD"};
//int main(void){

//	init_pins();
//	
//	lcd_init();
//	lcd_clear();
//	
//	lcd_goto(0,0);
//	lcd_write_string("Ashesi University");
//	lcd_goto(0,1);
//	lcd_write_string("Embedded Systems");
//	lcd_scroll_right(0, "Hannah Lormenyo" );
//	lcd_clear();
//	lcd_scroll_screen(0, "LCD Library " );
//	lcd_scroll_screen(0, text );
//	while (1){

//	
//	continue;}

//}



