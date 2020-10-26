#include <time.h>
// ***** 1. Pre-processor Directives Section *****
//  INCLUDES: Lists any external libraries we will refer to using 
//  a #include tag
#include <stdio.h>  // If a library is standard C library use the <> braces

//#include "uart.h"    // If the library is user defined use quotes " "

void delay(int number_of_seconds)
{
    // Converting time into milli_seconds 
    int milli_seconds = 1000 * number_of_seconds;

    // Storing start time 
    clock_t start_time = clock();

    // looping till required time is not achieved 
    while (clock() < start_time + milli_seconds);
}

long sum(long x, long y) {
    long z;
    z = x + y;
    return(z);
}
void main(void) {
    long a, b;
    int t;
    a = sum(2000, 2000);
    b = 0;
    t = 0;
    while (t<100) {
        b = sum(b, 1);
        //printf("%p",&b);
        printf("%li",b);
        t += 1;
        delay(1);
    }
}