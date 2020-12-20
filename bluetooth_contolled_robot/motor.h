#include "general.h"

#define MOTOR1_CONTROL1A 	(9)	// on port B- PTB9 - GPIO
#define MOTOR1_CONTROL2A	(8)		// on port B - PTB8 - GPIO
#define MOTOR1_ENABLE		(2)		//PTB2 to monitor on scope, setup to be output of TPM2_CH0
#define MOTOR2_CONTROL3A  (10)  // PTC10 - GPIO
#define MOTOR2_CONTROL4A (11)   // PTC11 - GPIO
#define MOTOR2_ENABLE (23) //TPM2_CH1
#define RED_LED_POS (18)		// on port B


void init_motor_control_lines(void);
void init_motor_timer(void);
void reverse_motors(void);
void forward_motors(void);
void turn_car_left(void);
void turn_car_right(void);
void increase_speed_progressively(void);
void stop_car(void);
void slow_down(void);
void move_faster(void);
