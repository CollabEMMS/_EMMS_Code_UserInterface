/******
 Common Header File
 Only put something in here if:
	it is required for all C files
	it is not best included in any other header file
 */

#ifndef COMMON_H
#    define	COMMON_H

#    define FCY 16000000UL


//#include <xc.h>
#    include <p24FV32KA301.h>
#    include <libpic30.h>
#    include <stdbool.h>

//							"          "  10 character spacing reference - do not exceed or the system will crash
#define MODULE_INFO_THIS_0	"User Int"		// 10 chars max name
#define MODULE_INFO_THIS_1	"2.1.2"			// 10 chars max version
#define MODULE_INFO_THIS_2	""				// 10 chars max info
#define MODULE_INFO_THIS_3	""				// 10 chars max info
#define MODULE_INFO_THIS_4	""				// 20 chars max info (only 18 will show on UI)


#    define CHAR_NULL '\0'

#    define BUF_SIZE_CHAR 5
#    define BUF_SIZE_INT 7
#    define BUF_SIZE_LONG 12


// can be used for debugging
#    define BACKLIGHT_NORMAL true // set to 'true' to use the backlight normally - anything else will allow debugging
#    define BACKLIGHT   _RB2    // Pin 6:  RB2

#define MODULE_INFO_SIZE_SMALL	11	// 10 + include the null char
#define MODULE_INFO_SIZE_LARGE	21	// 20 + include the null char
#define MODULE_COUNT			6	// 6 module info sets: 0=Command Board, 1=UART-1, 2=UART-2, 3=SPI-0, 4=SPI-1, 5=SPI-2
#define MODULE_INFO_COUNT		5	// 5 info items in each module

struct moduleInfo_struct
{
	char info0[MODULE_INFO_SIZE_SMALL];
	char info1[MODULE_INFO_SIZE_SMALL];
	char info2[MODULE_INFO_SIZE_SMALL];
	char info3[MODULE_INFO_SIZE_SMALL];
	char info4[MODULE_INFO_SIZE_LARGE];
};


#endif	/* COMMON_H */



/******
 Below are the comments for the headers of C files
 This should be placed at the top of each c file to provide some sense and
   structure to make things easier to understand
 */



/****************
 INCLUDES
 only include the header files that are required
 ****************/

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



