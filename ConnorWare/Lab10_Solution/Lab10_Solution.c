// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>

#include "../inc/PLL.h"
#include "../inc/SysTick.h"


#define VEH_LIGHT                   (*((volatile unsigned long *)0x400050FC))
#define GPIO_PORTB_OUT          (*((volatile unsigned long *)0x400050FC)) // Port B bits 5-0

#define GPIO_PORTE_IN           (*((volatile unsigned long *)0x4002400C)) // Port E bits 2-0
#define SENSORS                 (*((volatile unsigned long *)0x4002401C))

#define WALK_LIGHT (*((volatile unsigned long *)0x40025028))  //Port F bits 3 and 1
//bit-specific addressing (frinedly)
//green is 0x08 for WALK_LIGHT;
//red is 0x02;
//both off is 0X00;
//could also do masking (unfriendly or not)
//green is GPIO_PORTF_DATA_R &= ~0x02 to clear bit 1 and GPIO_PORTF_DATA_R |= 0x08 to set bit 3 only; or can just do = 0x08 (unfriendly);
//red is GPIO_PORTF_DATA_R &= ~0x08 to clear bit 3 and GPIO_PORTF_DATA_R |= 0x02 to set bit 1 only; or can just do = 0x02 (unfriendly);
//off is GPIO_PORTF_DATA_R &= ~0x0A to clear bits 1 and 3 or just do = 0x00 (unfriendly);

#define SYSCTL_RCGC2_GPIOE      0x00000010  // port E Clock Gating Control
#define SYSCTL_RCGC2_GPIOB      0x00000002  // port B Clock Gating Control



// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void PortE_Init(void);
void PortB_Init(void);
void PortF_Init(void);
unsigned long Sensor_Input(void);
void goSouth(void);
void waitSouth(void);
void goWest(void);
void waitWest(void);
void Walk(void);
void walkHurryOff(void);
void walkHurryOn(void);

// Linked data structure
struct State {
    void (*CmdPt)(void);   // output function
    unsigned long Time;
    unsigned long Next[8];
};
typedef const struct State STyp;
#define goS   0
#define waitS 1
#define goW   2
#define waitW 3
#define walk 4
#define wh1off 5
#define wh1on 6
#define wh2off 7
#define wh2on 8
STyp FSM[9] = {
/*goS */ {&goSouth,100,{goS,waitS,goS,waitS,waitS,waitS,waitS,waitS}}, 
/*waitS */ {&waitSouth, 100,{goW,goW,goW,goW,walk,walk,walk,goW}},
/*goW */ {&goWest,100,{goW,goW,waitW,waitW,waitW,waitW,waitW,waitW}},
/*waitW */ {&waitWest, 100,{goS,goS,goS,goS,walk,walk,walk,walk}},
/*walk */ {&Walk, 100,{walk,wh1off,wh1off,wh1off,walk,wh1off,wh1off,wh1off}},
/*wh1off */ {&walkHurryOff, 100,{wh1on,wh1on,wh1on,wh1on,wh1on,wh1on,wh1on,wh1on}},
/*wh1on */ {&walkHurryOn, 100,{wh2off,wh2off,wh2off,wh2off,wh2off,wh2off,wh2off,wh2off}},
/*wh2off */ {&walkHurryOff, 100,{wh2on,wh2on,wh2on,wh2on,wh2on,wh2on,wh2on,wh2on}},
/*wh2on*/ {&walkHurryOn, 100,{walk,goW,goS,goS,walk,goW,goS,goS}}
};
unsigned long S;  // index to the current state 
unsigned long Input;

// ***** 3. Subroutines Section *****

int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
  PLL_Init();       // 80 MHz, Program 10.1
  SysTick_Init();   // Program 10.2
  PortE_Init();   // initialize PE2-0 to input
  PortF_Init();   // initialize PF2-1 to output
  PortB_Init();   // initialize PB5-0 to output
  S = goS;
  while (1) {
      (FSM[S].CmdPt)();           // call output function
      SysTick_Wait10ms(FSM[S].Time);
      Input = Sensor_Input();       // input can be 0-7
      S = FSM[S].Next[Input];
  }
  
  EnableInterrupts();
  while(1){
     
  }
}


void PortE_Init(void) {
    volatile unsigned long delay;
    // maybe move these first two lines outside of the function
    SYSCTL_RCGC2_R |= 0x10;      // 1) activate clock for Port E
    delay = SYSCTL_RCGC2_R;      // 2) allow time for clock to start, no need to unlock
    GPIO_PORTE_AMSEL_R &= ~0x07; // 3) disable analog function on PE2-0
    GPIO_PORTE_PCTL_R &= ~0x000000FF; // 4) enable regular GPIO
    GPIO_PORTE_DIR_R &= ~0x07;   // 5) inputs on PE2-0
    GPIO_PORTE_AFSEL_R &= ~0x07; // 6) regular function on PE2-0
    GPIO_PORTE_DEN_R |= 0x07;    // 7) enable digital on PE1-0
}

void PortB_Init(void) {
    volatile unsigned long delay;
    SYSCTL_RCGC2_R |= 0x02;      // 1) activate clock for Port B
    delay = SYSCTL_RCGC2_R;      // 2) allow time for clock to start, no need to unlock
    GPIO_PORTB_AMSEL_R &= ~0x3F; // 3) disable analog function on PB5-0
    GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // 4) enable regular GPIO
    GPIO_PORTB_DIR_R |= 0x3F;    // 5) outputs on PB5-0
    GPIO_PORTB_AFSEL_R &= ~0x3F; // 6) regular function on PB5-0
    GPIO_PORTB_DEN_R |= 0x3F;    // 7) enable digital on PB5-0
}

void PortF_Init(void) {
    volatile unsigned long delay;
    SYSCTL_RCGC2_R |= 0x00000020;     // 1) activate clock for Port F
    delay = SYSCTL_RCGC2_R;           // allow time for clock to start
    // only PF0 needs to be unlocked, other bits can't be locked, bring unlock back in if having issues
    GPIO_PORTF_CR_R = 0x0E;           // allow changes to PF3-1
    GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
    GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF3-1
    GPIO_PORTF_DIR_R = 0x0E;          // 5) PF3-1 out
    GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
    GPIO_PORTF_DEN_R = 0x0E;          // 7) enable digital I/O on PF4-0
}

unsigned long Sensor_Input(void) {
    return SENSORS;  // PE2,1,0 can be 0-7
}

void goSouth(void) {
    VEH_LIGHT = 0x21;  
    WALK_LIGHT = 0x02;
}

void waitSouth(void) {
    VEH_LIGHT = 0x22;
    WALK_LIGHT = 0x02;
}

void goWest(void) {
    VEH_LIGHT = 0x0C;
    WALK_LIGHT = 0x02;
}

void waitWest(void) {
    VEH_LIGHT = 0x14;
    WALK_LIGHT = 0x02;
}

void Walk(void) {
    VEH_LIGHT = 0x24;
    WALK_LIGHT = 0x08;
}

void walkHurryOff(void) {
    VEH_LIGHT = 0x24;
    WALK_LIGHT = 0x00;
}

void walkHurryOn(void) {
    VEH_LIGHT = 0x24;
    WALK_LIGHT = 0x02;
}
