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
unsigned char timeYear_global;
unsigned char timeMonth_global;
unsigned char timeDay_global;
//unsigned char timeWeekday;
unsigned char timeHour_global;
unsigned char timeMinute_global;
unsigned char timeSecond_global;




// internal only


// not sorted yet
int resetTimeSecond_global;

char clockStr_global[6];
char calendarStr_global[9];


char timeSetPos_global;
char tempHour_global;
char tempMin_global;
char tempMonth_global;
char tempDay_global;
char tempYear_global;

/****************
 FUNCTION PROTOTYPES
 only include functions called from within this code
 external functions should be in the header
 ideally these are in the same order as in the code listing
 any functions used internally and externally (prototype here and in the .h file)
	 should be marked
 *****************/
void rtccReadTime( void );
void writeClockStrings( void );
//void writeTempClockStrings( void );
char writeTime( char newYear, char newMonth, char newDay, char newHour, char newMinute, char newSecond );

char BcdToDec( char val );
char DecToBcd( char val );

/****************
 CODE
 ****************/

/* readTime
 * reads time from RTCC, writes to global time variables
 */
void rtccReadTime( void )
{
	unsigned int temp;

	_RTCPTR = 0b11;
	timeYear_global = RTCVAL; //11
	temp = RTCVAL; //10
	timeMonth_global = temp >> 8;
	timeDay_global = temp & 0xFF;
	timeHour_global = RTCVAL & 0xFF; //01
	temp = RTCVAL; //00
	timeMinute_global = temp >> 8;
	timeSecond_global = temp & 0xFF;

	timeYear_global = BcdToDec( timeYear_global );
	timeMonth_global = BcdToDec( timeMonth_global );
	timeDay_global = BcdToDec( timeDay_global );
	timeHour_global = BcdToDec( timeHour_global );
	timeMinute_global = BcdToDec( timeMinute_global );
	timeSecond_global = BcdToDec( timeSecond_global );

	return;
}

char writeTime( char newYear, char newMonth, char newDay, char newHour, char newMinute, char newSecond )
{
	char returnValue;

	returnValue = 1;

	switch( tempMonth_global )
	{
		case 4:
		case 6:
		case 9:
		case 11:
			if( tempDay_global > 30 )
			{
				return -1;
			}
			break;

		case 2:
			if( tempYear_global % 4 )
			{
				if( tempDay_global > 28 )
				{
					returnValue = -1;
				}
			}
			else
			{
				if( tempDay_global > 29 )
				{
					returnValue = -1;
				}
			}
			break;
	}

	// put values into BCD
	newYear = DecToBcd( newYear );
	newMonth = DecToBcd( newMonth );
	newDay = DecToBcd( newDay );
	newHour = DecToBcd( newHour );
	newMinute = DecToBcd( newMinute );
	newSecond = DecToBcd( newSecond );

	_RTCEN = 0; // disable clock

	//does unlock sequence to enable write to RTCC, sets RTCWEN
	__builtin_write_RTCWEN( );

	_RTCPTR = 0b11; // decrements with read or write
	RTCVAL = newYear; // set year
	RTCVAL = ( ( (int) newMonth ) << 8 ) + newDay; // set month, day
	RTCVAL = newHour; // clear weekday, set hour
	RTCVAL = ( ( (int) newMinute ) << 8 ) + newSecond; // set minute, second

	_RTCEN = 1; // enable clock

	_RTCWREN = 0; // Disable Writing

	resetTimeSecond_global = 59;

	return returnValue;
}

char BcdToDec( char val )
{
	char tempValue;

	tempValue = ( val / 16 * 10 ) + ( val % 16 );

	return tempValue;
}

char DecToBcd( char val )
{
	char tempValue;
	
	tempValue = ( val / 10 * 16 ) + ( val % 10 );
	
	return tempValue;
}
