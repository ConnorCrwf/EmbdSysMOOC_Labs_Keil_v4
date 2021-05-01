// LineTracker.c
// Runs on LM4F120 or TM4C123
// Index implementation of a Moore finite state machine to operate
// a line-tracking robot.
// Daniel Valvano, Jonathan Valvano
// April 24, 2014

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013
   Volume 1 Program 6.8, Example 6.4
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013
   Volume 2 Program 3.1, Example 3.1

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
#include "..//tm4c123gh6pm.h"
#include "PLL.h"
#include "SysTick.h"
// The robot has two drive wheels and two line sensors

void Robot_Init(void);                   // Initialize GPIO used for robot
unsigned long Robot_Input(void);         // Input from sensors
void Robot_Output(unsigned long output); // Output to motors

// Linked data structure
struct State {
  unsigned long out;        // 2-bit output 
  unsigned long delay;      // time to delay in 10ms
  unsigned long next[4];};  // Next if 2-bit input is 0-3
    //00 means lost pick a direction
    //01 means off to right
    //10 means off to left
    //11 means on right path
typedef const struct State StateType;
typedef StateType *StatePtr;
#define Center 0
#define Left 1
#define Right 2
//if off to left, you want to turn on Left motor (0x01, in other words a 1 at PF1)
//if off to right, you want to turn on Right Motor (0x02, in other words a 1 at PF2)
//so right bit of Output (PF1) address must be connected to the left motor
//and the left bit of the Output address (PF2) must be connected to right motor
StateType fsm[3]={
  {0x03, 1, { Right, Left,   Right,  Center }},  // Center
  {0x01, 1, { Left,  Center, Right,  Center }},  // Left State, Output is Turn Right (High PF1)
  {0x02, 1, { Right, Left,   Center, Center }}   // Right State, Output is Turn Left (High PF2)
};

unsigned long S;       // index to the current state 
unsigned long Input; 
unsigned long Output;
int main(void){ 
  Robot_Init();
  SysTick_Init();   // Program 10.2
  S = Center;  
  while(1){
    Output = fsm[S].out;      // set output from FSM
    Robot_Output(Output);     // do output to two motors
    SysTick_Wait10ms(fsm[S].delay); // wait
    Input = Robot_Input();    // read sensors
    S = fsm[S].next[Input];   // next depends on input and state 
  }
}

// ********Robot_Init***************
// Initialize GPIO used for robot
void Robot_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
  GPIO_PORTF_PUR_R = 0x11;          // enable pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital I/O on PF4-0
}

// ********Robot_Input***************
//if over a white space the sensor will go to zero
// Remember that PF0 and PF4 will remain high by default due to PUR.
// If want actual robot to work like this should make it so that you have a low signal returned when go over white space and not a high signal
// PF4 is the left bit of the input, PF0 is the right bit.
// PF4 is the left side sensor (that 2nd bit goes to zero when too far left, result in Input of 0x01 or 00000001)
// PF0 is the right side sensor (that 1st bit goes to zero when too far right, result in Input of 0x02 or 00000002)
// Input from sensors
// 0,0		Lost, off the line
// 0,1 or 0x01	off to left
// 1,0 or 0x02		off to right
// 1,1 or 0x03 	on line
//This gives you just PF4 and PF0
#define INPUT (*((volatile unsigned long *)0x40025044))
unsigned long test1, test2, test3;
unsigned long Robot_Input(void){
  // combine PF4 (left sensor) and PF0 (right sensor) into 2 adjacent bits
	test1 = INPUT&0x01;
	test2 = (INPUT>>3)&0x02;
	test3 = ((INPUT&0x01)+((INPUT>>3)&0x02));
	//mask everything but PF0 then right shift PF4 to PF1 position and then mask everything but PF1
  return ((INPUT&0x01)+((INPUT>>3)&0x02));
}

// ********Robot_Output***************
// Output to motors
// 
// Right motor is PF2 or left bit of bit-specific address for PF1-2
// Left motor is PF1 or right bit of bit-specific address for PF1-2
// 1,1 If both motors are on, the robot goes straight. PF1 and PF2 high to turn both motors on when in a Center State
// PF1 is connected to left motor to turn it right when in a Left State -> 0,1 or 0x01 for Output
// PF2 is connected to right motor to turn it left when in a Right State -> 1,0 ox 0x02 for Output
#define OUTPUT (*((volatile unsigned long *)0x40025018))
void Robot_Output(unsigned long output){
  OUTPUT = output<<1; // set PF2,PF1
}
