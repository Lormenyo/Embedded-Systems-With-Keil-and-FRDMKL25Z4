# Periodic Interrupt Handler

In the pit.c file, PIT was used to blink an LED every 400ms. After blinking 5 times, the blinking rate is changed to 100ms  for 5x and the process is repeated.

NB: Blinking 5x is 10 toggles.[on, off, on, off, ....]

## What is the Periodic Interrupt Handler
- The PIT uses the Bus clock which uses a frequency that is half of the System Clock's frequency.
- The system clock has a frequency of 20971520 Hz, hence the frequeny of PIT bus clock is 20971520 * 0.5 = 10485760

## Configuring the PIT
- Enable the clock to the PIT module
```
SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
```

- Enable the MDIS module
```
PIT->MCR &= ~PIT_MCR_MDIS_MASK;
```
- Initialise the PIT channel to count down from the starting value(Load Value)
```
PIT->CHANNEL[0].LDVAL |= starting_value;
```
The code above loads the starting value into the LDVAL register for channel 0.

- Enable interrupts from the PIT. The example below enables interrupts for channel 0.
```
PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;
```

- Enable the channel Timer. The example below enables the timer for channel 0.
```
PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
```

- Configure NVIC
1. Set PIT IRQ priority
 ```
 NVIC_SetPriority(PIT_IRQn, 3); 
```
2. Clear any pending IRQ from PIT
```
NVIC_ClearPendingIRQ(PIT_IRQn); 
```
            
3. Enable the PIT interrupt in the NVIC
 ```
NVIC_EnableIRQ(PIT_IRQn);
 ```
            
## Calculating the LOADVALUE 
- The LoadValue is the value that the PIT counts down from. 
- Assuming you want an interrupt every T seconds, the LoadValue V can be calculated using this formula:

          V = T * 10485760 - 1
          
 ## Generating the interrupts
 - ```PIT_IRQHandler``` is the interrupt handler for PIT.
 - To do something for every interrupt,
 1. Check if the channel interrupt flag has been raised
 ```
 if (PIT->CHANNEL[n].TFLG & PIT_TFLG_TIF_MASK) {
  }
```
n is the channel number [n=0,1]

 2. If yes, do something like toggling the state of an LED in this case.
  ```
 if (PIT->CHANNEL[n].TFLG & PIT_TFLG_TIF_MASK) {
    PTB -> PTOR |= 1UL << LED;
  }
  ```
n is the channel number [n=0,1]

 3. After that clear the flag.
  ```
 if (PIT->CHANNEL[n].TFLG & PIT_TFLG_TIF_MASK) {
    PTB -> PTOR |= 1UL << LED;
    PIT->CHANNEL[n].TFLG &= PIT_TFLG_TIF_MASK;

  }
  ```
n is the channel number [n=0,1]

 - PIT has 2 channels. I used one channel for the 400ms interrupts and the other channel for the 100ms interrupts. 
 - I kept track of the num of toggles and after every 10 toggles, i disable the channel being used and switch to a different channel.
 - Other methods can be explored. You can load a different value to the LoadVal register everytime you want a different rate.
