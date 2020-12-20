#include "general.h"

volatile unsigned long counter=0;

void SysTick_Handler(){
	counter++; //Counting the number of millis
}

void systick_clock(){
 SysTick->LOAD = (20971520u/1000u)-1 ; //configure for every milli sec restart.
 SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk |SysTick_CTRL_TICKINT_Msk; 
}

unsigned long millis(){
return (unsigned long)counter;
}
