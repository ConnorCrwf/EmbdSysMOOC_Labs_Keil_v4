#include <time.h>
// ***** 1. Pre-processor Directives Section *****
//  INCLUDES: Lists any external libraries we will refer to using 
//  a #include tag
#include <stdio.h>  // If a library is standard C library use the <> braces

unsigned long error;
// Calculates area
// Input:  side of a room (unsigned long)
// Output: area of the room (unsigned long)
// Notes: ...
unsigned long Calc_Area(unsigned long s) {
    unsigned long result;
    if (s <= 25) {
        result = s * s;
    }
    else {
        result = 0; // mistake
        error = error + 1;
    }
    return(result);
}

int main(void) {
    unsigned long side; // room wall meters
    unsigned long area; // size squared meters
    //UART_Init();  // call subroutine to initialize the uart
    printf("This program calculates areas of square-shaped rooms\n");
    side = 1;
    //while (side < 50) {
    //    area = calc_area(side);
    //    printf("\narea of the room with side of %ld m is %ld sqr m\n", side, area);
    //    side = side + 1;
    //}
    for (side = 1; side < 50; side = side + 1) {
        area = Calc_Area(side);
        printf("\nArea of the room with side of %ld m is %ld sqr m\n", side, area);
    }
}