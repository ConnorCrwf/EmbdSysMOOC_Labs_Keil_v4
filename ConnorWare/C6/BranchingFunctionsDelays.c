// BranchingFunctionsDelays.c Lab 6
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to 
// toggle an LED while a button is pressed and 
// turn the LED on when the button is released.  
// This lab will use the hardware already built into the LaunchPad.
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// built-in connection: PF0 connected to negative logic momentary switch, SW2
// built-in connection: PF1 connected to red LED
// built-in connection: PF2 connected to blue LED
// built-in connection: PF3 connected to green LED
// built-in connection: PF4 connected to negative logic momentary switch, SW1

#include "TExaS.h"

#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control

#define PF4 (*((volatile unsigned long *)0x40025040))
#define PF2 (*((volatile unsigned long *)0x40025010))

// Declaration Section
//Global Variables
unsigned long In;  // input from PF4

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
// function prototypes
void PortF_Init(void);    //Initialize Port F
void Delay100(void);         //Delay


int main(void){ unsigned long volatile delay;
  TExaS_Init(SW_PIN_PF4, LED_PIN_PF2);  // activate grader and set system clock to 80 MHz
  // initialization goes here
	//		1) Make PF2 an output and make PF4 an input (enable PUR for PF4).
	PortF_Init();        // Call initialization of port PF4 PF2   
  EnableInterrupts();           // enable interrupts for the grader
	//		2) The system starts with the LED ON (make PF2 =1).
//	GPIO_PORTF_DATA_R = 0x04;    // LED is blue
	//bit-specific
	PF2 = 0x04;
	
  while(1){
		// body goes here
		//		3) Delay for about 100 ms
			Delay100(); 
//			Delay20(); 
		//		4) If the switch is pressed (PF4 is 0), then toggle the LED once, else turn the LED ON.
		  if(PF4 == 0x00){              // zero means SW1 is pressed

				//add in a delay to make it flash; delay at start of while loop handles that
				PF2 ^= 0x04;
			}
			else
			{
				PF2 = 0x04;
			}
		}
  }

//OBSOLETE CODE	, non-friendly
//			In = GPIO_PORTF_DATA_R&0x10; // read PF4 into In
//      GPIO_PORTF_DATA_R = 0x00;    // LED is off
//				GPIO_PORTF_DATA_R = 0x04;    // LED is blue	


void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
	//direction register for input pin is a 0 bit, for an output pin is a 1 bit
	// use hex to binary converter to make this visualization easier
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output; same as 00001110  
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0; same as 00011111        
}

// Subroutine to wait 0.1 sec
// Inputs: None
// Outputs: None
// Notes: ...
void Delay100(void){unsigned long volatile time;
  time = 727240*200/91;  // 0.1sec
  while(time){
		time--;
  }
}
