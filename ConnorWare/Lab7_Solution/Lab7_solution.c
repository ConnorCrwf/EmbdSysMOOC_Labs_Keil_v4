// 0.Documentation Section 
// Lab7_HeartBlock, main.c

// Runs on LM4F120 or TM4C123 LaunchPad
// Input from PF4(SW1) is AS (atrial sensor), 
// Output to PF3, Green LED, is Ready,
// Output to PF1, Red LED, is VT (ventricular trigger) 
// Make PF4 input, PF3,PF1 output
// Initialize Ready to high and VT to low
// Repeat this sequence of operation over and over
// 1) Wait for AS to fall (touch SW1 switch)
// 2) Clear Ready low
// 3) Wait 10ms (debounces the switch)
// 4) Wait for AS to rise (release SW1)
// 5) Wait 250ms (simulates the time between atrial and ventricular contraction)
// 6) set VT high, which will pulse the ventricles 
// 7) Wait 250ms
// 8) clear VT low 
// 9) set Ready high

// Date: January 15, 2016

// 1. Pre-processor Directives Section
#include "TExaS.h"

// Constant declarations to access port registers using 
// symbolic names instead of addresses
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
// 2. Declarations Section
//   Global Variables
//AS
#define PF4 (*((volatile unsigned long *)0x40025040))
//Ready
#define PF3 (*((volatile unsigned long *)0x40025020))
//VT
#define PF1 (*((volatile unsigned long *)0x40025008))
	
unsigned long LED_R;

//   Function Prototypes
void PortF_Init(void);
void Delay1ms(unsigned long msecs);
void Delay1ms_RealBoard(unsigned long msecs);
void DelayHalfSec(unsigned long halfsecs);
void EnableInterrupts(void);  // Enable interrupts
void WaitForASLow(void);
void WaitForASHigh(void);
void SetVT(void);
void ClearVT(void);
void SetReady(void);
void ClearReady(void);

// 3. Subroutines Section
// MAIN: Mandatory for a C Program to be executable
int main(void){
  TExaS_Init(SW_PIN_PF40, LED_PIN_PF31,ScopeOn);  // activate grader and set system clock to 80 MHz
  PortF_Init();                            // Init port PF4 PF3 PF1    
  EnableInterrupts();                      // enable interrupts for the grader  
  while(1){          // Follows the nine steps list above
    // a) Ready signal goes high
    // b) wait for switch to be pressed
    // c) Ready signal goes low
    // d) wait 10ms
    // e) wait for switch to be released
    // f) wait 250ms
    // g) VT signal goes high
    SetReady();
		WaitForASLow();
    ClearReady();
		Delay1ms_RealBoard(10);
		WaitForASHigh();
		Delay1ms_RealBoard(250);
	SetVT();
	Delay1ms_RealBoard(250);
	ClearVT();
//	Delay1ms(2000);
    // h) wait 250ms
    // i) VT signal goes low
  }
}
// Subroutine to initialize port F pins for input and output
// PF4 is input SW1 and PF3-1 is output LEDs
// Inputs: None
// Outputs: None
// Notes: ...
void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;      // 1) F clock
  delay = SYSCTL_RCGC2_R;            // delay to allow clock to stabilize     
  GPIO_PORTF_AMSEL_R &= 0x00;        // 2) disable analog function
  GPIO_PORTF_PCTL_R &= 0x00000000;   // 3) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R &= ~0x10;         // 4.1) PF4 input,
  GPIO_PORTF_DIR_R |= 0x0E;          // 4.2) PF3,2,1 output  
  GPIO_PORTF_AFSEL_R &= 0x00;        // 5) no alternate function
  GPIO_PORTF_PUR_R |= 0x10;          // 6) enable pullup resistor on PF4       
  GPIO_PORTF_DEN_R |= 0x1E;          // 7) enable digital pins PF4-PF1
}
// Color    LED(s) PortF
// dark     ---    0
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08
// yellow   RG-    0x0A
// sky blue -GB    0x0C
// white    RGB    0x0E


// Subroutine reads AS input and waits for signal to be low
// If AS is already low, it returns right away
// If AS is currently high, it will wait until it to go low
// Inputs:  None
// Outputs: None
void WaitForASLow(void){
// write this function
	unsigned long SW1;
	do{
      SW1 = GPIO_PORTF_DATA_R&0x10; // PF4 into SW1, 
    }while(SW1 == 0x10);
}

// Subroutine reads AS input and waits for signal to be high
// If AS is already high, it returns right away
// If AS is currently low, it will wait until it to go high
// Inputs:  None
// Outputs: None
void WaitForASHigh(void){
// write this function
	unsigned long SW1;
	do{
      SW1 = GPIO_PORTF_DATA_R&0x10; // PF4 into SW1, clears all the bits we don't care about in SW1 variable, leaving only the PF4 bit we care about.
    }while(SW1 != 0x10);
}

// Subroutine sets VT high
// Inputs:  None
// Outputs: None
// Notes:   friendly means it does not affect other bits in the port
void SetVT(void){
	//PF1 = 0x02;
	GPIO_PORTF_DATA_R |= 0x02;  
// write this function
	
}

// Subroutine clears VT low
// Inputs:  None
// Outputs: None
// Notes:   friendly means it does not affect other bits in the port
void ClearVT(void){
// write this function
	//PF1 = 0x00;
	GPIO_PORTF_DATA_R &= ~0x02;
}

// Subroutine sets Ready high
// Inputs:  None
// Outputs: None
// Notes:   friendly means it does not affect other bits in the port
void SetReady(void){
// write this function
    GPIO_PORTF_DATA_R |= 0x08;
}


// Subroutine clears Ready low
// Inputs:  None
// Outputs: None
// Notes:   friendly means it does not affect other bits in the port
void ClearReady(void){
// write this function
    GPIO_PORTF_DATA_R &= ~0x08;
}


// Subroutine to delay in units of milliseconds
// Inputs:  Number of milliseconds to delay
// Outputs: None
// Notes:   assumes 80 MHz clock
void Delay1ms(unsigned long n) {
    unsigned long volatile time;
    while (n) {
        time = 72724 * 2 / 91;  // 1msec, tuned at 80 MHz
        while (time) {
            time--;
        }
        n--;
    }
}

void Delay1ms_RealBoard(unsigned long msecs) {
    unsigned long volatile time;
    while (msecs > 0) {
        time = 16000*25/30;  // 1msec, tuned at 80 MHz
        while (time) {
            time--;
        }
        msecs--;
    }
}

void DelayHalfSec(unsigned long halfsecs) {
    unsigned long count;

    while (halfsecs > 0) { // repeat while there are still halfsecs to delay
        count = 1538460; // 400000*0.5/0.13 that it takes 0.13 sec to count down to zero
        while (count > 0) {
            count--;
        } // This while loop takes approximately 3 cycles
        halfsecs--;
    }
}
