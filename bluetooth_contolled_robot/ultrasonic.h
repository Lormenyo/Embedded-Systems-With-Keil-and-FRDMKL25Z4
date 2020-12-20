#define ECHO_PIN (0) //PTB0 - TPM1_CH0 -ALT3
#define TRIGGER_PIN (0) //PTD0 - TPM0_CH0 ALT4
#define BUZZER (7) //PTD7


void init_ultrasonic_pins(void);
void init_ultrasonic_timer(void);
void raise_obstacle_alarm(void);
