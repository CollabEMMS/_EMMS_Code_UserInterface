/****************
 INCLUDES
 only include the header files that are required
 ****************/
#include "common.h"

/****************
 MACROS
 ****************/

/****************
 VARIABLES
 these are the globals required by only this c file
 there should be as few of these as possible to help keep things clean
 variables required by other c functions should be here and also in the header .h file
 as external
 ****************/

// external

// internal only


/****************
 FUNCTION PROTOTYPES
 only include functions called from within this code
 external functions should be in the header
 ideally these are in the same order as in the code listing
 any functions used internally and externally (prototype here and in the .h file)
     should be marked
 *****************/

/****************
 CODE
 ****************/
void resetWDT( void )
{
    asm( "CLRWDT" );
}