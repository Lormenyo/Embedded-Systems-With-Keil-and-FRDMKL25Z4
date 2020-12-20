#define EMERGENCY_BLUE_LED (5) //PTC5
#define EMERGENCY_RED_LED  (6) //PTC6
#define POTENTIOMETER_PIN (20) //PTE20
#define POTENTIOMETER_LED  (6)  //PTD6
#define TAIL_RED_LED (3)		//PTC3
#define TAIL_WHITE_LED (4)  //PTC4

void flashEmergencyLights(void);
void init_leds(void);
void init_ADC(void) ;
int measure_POT(void);
void init_pit(void);

