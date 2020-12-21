#include "motor.h"
#include "led.h"
#include "lcd.h"
#include "ultrasonic.h"
#include "queue.h"
#include "serial.h"

void carDefaultRoutine();

extern unsigned char distance;
char buffer[10];
extern unsigned int interval;
extern Q_T TxQ, RxQ;

int forward = 1;


int main(){
	Q_Init(&TxQ);
	Q_Init(&RxQ);
	init_bluetooth_pins();
	Init_UART();
	systick_clock();
	init_motor_control_lines();
	init_motor_timer();
	init_leds();
	init_lcd_pins();
	init_ultrasonic_pins();
	init_ultrasonic_timer();
	
	lcd_init();
	lcd_clear();
	lcd_goto(0,0);
	lcd_write_string("Ultrasonic Sensor");
	
	while(1){
		doTxRx_task();
		flashEmergencyLights();
		//potentiometer_value = measure_POT();
		
		distance = interval * 0.10468;
		sprintf(buffer, "Distance: %d cm", distance);
		lcd_goto(0,1);
		lcd_write_string(buffer);
		raise_obstacle_alarm();
		//carDefaultRoutine();
		forward_motors();
		slow_down();
		//stop_car();
		//forward_motors();
		//move_faster();
	}
}


void carDefaultRoutine(){
	static enum car_states {S1, S2, S3, S4, S5, S6, S7} next_state=S1;
	unsigned long current_time;
	static unsigned long last_run=0;
	
	current_time=millis();
	switch (next_state){
		case S1:
			forward_motors();
			if ((current_time-last_run)>=500){
				last_run=current_time;
				next_state=S2;
			}
			break;
		case S2:
			reverse_motors();
			if ((current_time-last_run)>=500){
				last_run=current_time;
				next_state=S3;
			}
			break;
			case S3:
			forward_motors();
			if ((current_time-last_run)>=500){
				last_run=current_time;
				next_state=S4;
			}
			break;
		case S4:
			turn_car_right();
			if ((current_time-last_run)>=500){
				last_run=current_time;
				next_state=S5;
			}
			break;
		case S5:
			forward_motors();
			if ((current_time-last_run)>=500){
				last_run=current_time;
				next_state=S6;
			}
			break;
		case S6:
			turn_car_left();
			if ((current_time-last_run)>=500){
				last_run=current_time;
				next_state=S7;
			}
			break;
		case S7:
			stop_car();
			if ((current_time-last_run)>=500){
				last_run=current_time;
				next_state=S1;
			}
			break;
		default:
			next_state =S7;
			break;
	}	
}
