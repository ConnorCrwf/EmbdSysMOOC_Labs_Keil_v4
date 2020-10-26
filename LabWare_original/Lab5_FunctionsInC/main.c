// ***** 0. Documentation Section *****
// main.c
//     Runs on TM4C123
//     UART runs at 115,200 baud rate 
//     Daniel Valvano
//     June 17, 2013
// Lab 5
//      Jon Valvano and Ramesh Yerraballi
//      January 15, 2016

// ***** 1. Pre-processor Directives Section *****
#include <stdio.h>   // standard C library 
#include "uart.h"    // functions to implment input/output
#include "TExaS.h"   // Lab grader functions

// ***** 2. Global Declarations Section *****
unsigned long error = 0;

// FUNCTION PROTOTYPES: Each subroutine defined
unsigned long Calc_Area(unsigned int l, unsigned int w); 
void EnableInterrupts(void);  // Enable interrupts
void Initialize(void);  // says Initialize takes no input returns none

// ***** 3. Subroutines Section *****
int main (void) {
  unsigned int length, width, area;
  TExaS_Init(UART_PIN_PA0,UART_PIN_PA1); // this initializes the TExaS grader lab 5
  UART_Init();    // initialize UART for printing
  printf("\nThis program calculates areas of rectangular rooms.\n");
  EnableInterrupts();  // the grader needs interrupts
  while(1) {
    printf("\nGive length: "); scanf("%i", &length);  // Get input
    printf("\nGive width: ");  scanf("%i", &width);   // Get input
    area = Calc_Area(length, width);
    printf("\nArea of the room = %i\n",  area);
  }
}

void Initialize(void) {
  error = 0;      // Access global error count and set it to zero
}

// Computes the Area of a rectangular room whose sides are given
// Input:  the length (l) and width (w) are the dimensions of a rectangle
// Output: the area of the rectangular room 
// Notes:  Return a result of zero if 
//    the length is less than 3,  
//    the width is less than 3,  
//    the length is greater than 20 or  
//    the width is greater than 20. 
unsigned long Calc_Area(unsigned int l, unsigned int w) {
  unsigned int result;

  unsigned int d[2];	
	unsigned int i;
  d[0] = l;
  d[1] = w;
  for (i = 0; i < 2; i++)
  {
		printf("\n Dimension is: %i", d[i]);
	if (d[i] >= 3 && d[i] <= 20) {
		printf("\nValid dim");
   }
	else { 
		printf("\nInvalid dim");
  }
}
 

// Put your Lab 5 code here
  if (l >= 3 && l <= 20) {
      if (w >= 3 && w <= 20)
      {
          result = l * w;    // compute valid area
      }
      else {
          result = 0;
          error = error + 1;
      }
  }
  else {
      result = 0;       // indicate invalid area
      error = error + 1;  // increment gloabal error count 
  }
  return(result);
}
