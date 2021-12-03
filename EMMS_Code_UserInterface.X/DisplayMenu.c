/****************
 INCLUDES
 only include the header files that are required
 ****************/
#include <stdlib.h>
#include "common.h"

#include "Communications.h"
#include "DisplayRTCC.h"
#include "Delays.h"
#include "DisplayMenuMacros.h"
#include "DisplayMain.h"

/****************
 MACROS
 ****************/
// If you want seconds on the home screen, uncomment next line
#define SECONDS_CLOCK

#define LCD_E       _RB9    // Pin 13: RB9
#define LCD_RS      _RB8    // Pin 12: RB8
#define LCD_D7      _RB15   // Pin 18: RB15
#define LCD_D6      _RB14   // Pin 17: RB14
#define LCD_D5      _RB13   // Pin 16: RB13
#define LCD_D4      _RB12   // Pin 15: RB12

/****************
 VARIABLES
 these are the globals required by only this c file
 there should be as few of these as possible to help keep things clean
 variables required by other c functions should be here and also in the header .h file
 as external
 ****************/
// external

char passwordSet[6];

long previousDayUsed = 0;
long totalUsed = 0;

// internal only


// not yet sorted
const char softKeys0[] = "Back  Up  Down    OK";
const char softKeys1[] = "Back  Up  Top     OK";
const char softKeys2[] = "Back  Up  Down  Save";

char audibleAlarm;
char alarm1Enabled;
char alarm2Enabled;
char alarm1Energy;
char alarm2Energy;
char emerButtonEnable;

int resetTimeHour;
int resetTimeMinute;
int tempResetHour;
int tempResetMinute;


char isHigh = 0xFF;
char tempIsHigh;
char oldPowerMenuState;


int emerButtonEnergyAllocate;
int emerButtonAllocTemp;
int emerAllocNow;
int emerAllocSend;

char powerDownTime[12];
char powerUpTime[12];

char powerBoxCodeVersionString[9];

char passwordInput[6];
char newPassword[12];
unsigned char pwLength;

char homeBasicAlternate; // 0 for % remaining, 1 for % load

char barGraph[21];

char tempResetTimeString[6];

unsigned char timeRemHour;
unsigned char timeRemMinute;
unsigned char timeRemSecond;
unsigned char timeRemUpdate;
unsigned long timeRem1;
unsigned long timeRem2;
unsigned long timeRem3;
unsigned long timeRem4;
unsigned long timeRem5;

char timeRemainingString[9];

char currentDisplay[NUM_LCD_LINES * NUM_LCD_WIDTH + 1];
char nextDisplay[NUM_LCD_LINES * NUM_LCD_WIDTH + 1];

unsigned char menuState;
unsigned char oldMenuState;
unsigned char lastUpdateSecond;
unsigned char lastUpdateMinute;
char enablePeriodicUpdate;

char activeAlarm;
char alarmToResume;
char rightArrow[2];
char tempPercent;
char silenceAlarmOne;
char silenceAlarmTwo;
char remainingSets;

unsigned long powerLoad;
unsigned long energyUsed;
long energyAllocated;
unsigned long lowAlloc;
unsigned long highAlloc;
unsigned long tempAlloc;

char percentRem;

unsigned char button0Flag;
unsigned char button1Flag;
unsigned char button2Flag;
unsigned char button3Flag;

char relayActive;
char tempRelayActive;

char isBooting;

char alarmOneHit;
char alarmTwoHit;



/****************
 FUNCTION PROTOTYPES
 only include functions called from within this code
 external functions should be in the header
 ideally these are in the same order as in the code listing
 any functions used internally and externally (prototype here and in the .h file)
	 should be marked
 *****************/
void dspCommand( unsigned char );
void dspWrite( unsigned char );
void dspNibble( void );
void initDisplay( void );
void dspSetP1( unsigned char );
void writeToDisplay( const char*, unsigned char, char );
void dspWriteMessage( char* );

void updateMenu( void );
void menuError( void );
void menuDebug( void );
void menuHomeBasic( void );
void menuHomeDetail( void );
void menuAlarm( void );
void menuMain1( void );
void menuMain2( void );
void menuMain3( void );
void menuMain4( void );
void menuMain5( void );
void menuMain6( void );
void menuAlarm1( void );
void menuAlarm2( void );
void menuAlarm3( void );
void menuAlarm4( void );
void menuAlarm5( void );
void menuAlarm6( void );
void menuAlarm7( void );
void menuAbout( void );
void menuAdminLogin( void );
void menuAdmin1( void );
void menuAdmin2( void );
void menuAdmin3( void );
void menuAdmin4( void );
void menuAdmin5( void );
void menuAdmin6( void );
void menuSetTime( void );
void menuSetPower( void );
void menuSetResetTime( void );
void menuStatistics( void );
void menuPowerFailTimes( void );
void menuEmergency1( void );
void menuEmergency2( void );
void menuEmergency3( void );
void menuEmergency4( void );
void menuModules( void );
void menuSetRelay( void );
void menuBadDate( void );
void menuShutOffWarning( void );
void menuPasswordChange( void );
void menuPowerReset( void );
void writeBarGraph( void );

/****************
 CODE
 ****************/

/* command
 * sends a command to the LCD
 * parameter i: char representing the command to be sent
 */
void dspCommand( unsigned char i )
{

	unsigned char lowNibble;
	unsigned char highNibble;

	lowNibble = i & 0x0F;
	highNibble = i >> 4;

	dspSetP1( highNibble );

	LCD_RS = 0;

	dspNibble( ); // Send lower 4 bits
	dspSetP1( lowNibble ); // put data on output Port
	dspNibble( ); // Send upper 4 bits
	delayMS( 2 );
}

/* write
 * sends a byte of data to the LCD to be displayed
 * parameter i: char to be displayed
 */
void dspWrite( unsigned char i )
{
	unsigned char lowNibble;
	unsigned char highNibble;

	lowNibble = i & 0x0F;
	highNibble = i >> 4;

	dspSetP1( highNibble ); //put data on output Port

	LCD_RS = 1;

	dspNibble( ); // Clock lower 4 bits
	dspSetP1( lowNibble ); // put data on output Port
	dspNibble( ); // Clock upper 4 bits
}

/* writeMessage
 * sends a string of data to the LCD to be displayed
 * end of data will be denoted by a null character (0x00)
 * parameter *message: pointer to string to be displayed
 *     (char array ending with null)
 */
void dspWriteMessage( char *message )
{
	char *pos = message;
	while( *pos != 0 )
		dspWrite( *pos++ );
}

/* nibble
 * sends half of a byte of data to the LCD, already on output lines to LCD
 */
void dspNibble( void )
{
	LCD_E = 1;
	delayFourUS( 1 ); //enable pulse width >= 300ns
	LCD_E = 0; //Clock enable: falling edge
}

/* initDisplay
 * initializes the LCD for 4-bit operation
 */
void initDisplay( void )
{
	dspSetP1( 0 );

	delayMS( 100 ); //Wait >15 msec after power is applied
	dspSetP1( 3 ); //put 0x30 on the output port
	delayMS( 30 ); //must wait 5ms, busy flag not available

	dspNibble( ); //command 0x30 = Wake up
	delayMS( 10 ); //must wait 160us, busy flag not available
	dspNibble( ); //command 0x30 = Wake up #2
	delayMS( 10 ); //must wait 160us, busy flag not available
	dspNibble( ); //command 0x30 = Wake up #3
	delayMS( 10 ); //can check busy flag now instead of delay
	dspSetP1( 2 ); //put 0x20 on the output port
	dspNibble( ); //Function set: 4-bit interface

	//    command(0b00100000);      //Function set: 4-bit/2-line
	//    command(0b00101100);      //Function set: 4-bit/2-line
	//    command(0b00001010);      //Cursor on
	//    command(0b00010000);      //Set cursor
	//    command(0b00000001);      //Clear Display
	//    command(0b00000110);      //Entry Mode set
	//    command(0b00001100);      //Display on, cursor off


	dspCommand( 0x28 ); //Function set: 4-bit/2-line
	dspCommand( 0x10 ); //Set cursor
	dspCommand( 0b00001100 ); //Display ON; Blinking cursor off
	dspCommand( 0x06 ); //Entry Mode set

	dspCommand( 0b00001100 ); //Cursor off
	dspCommand( CLEAR_DISPLAY ); //Clear Display
	delayMS( 2 );
	dspCommand( FIRST_LINE ); //Return To First Line

	rightArrow[0] = 0x7E; // right arrow character for menus
	rightArrow[1] = 0x00;

	unsigned char i;

	if( BACKLIGHT_NORMAL == true )
	{
		BACKLIGHT = 1; // turn on backlight
	}

	// initialize starting arrays to 0
	for( i = 0; i < NUM_LCD_LINES * NUM_LCD_WIDTH; i++ )
	{
		currentDisplay[i] = ' ';
		nextDisplay[i] = ' ';
	}
}

/* setP1
 * prepares output lines to LCD for data transfer
 * parameter i: low nibble (lowest 4 bits) in i are data to be sent
 */
void dspSetP1( unsigned char i )
{
	LCD_D4 = i % 2;
	i = i >> 1;
	LCD_D5 = i % 2;
	i = i >> 1;
	LCD_D6 = i % 2;
	i = i >> 1;
	LCD_D7 = i % 2;

}

/* writeToDisplay
 * writes a message to the nextDisplay array to await transfer to the LCD
 * can pad string with blank characters to either side to right- or left-align
 * writes to specific part of nextDisplay, relating to specific part of the LCD
 * (for 4x20 0 is beginning of first line, 20 is beginning of second, 40, etc.)
 *
 * parameter *message: pointer to message to be put into nextDisplay
 *     works with a char pointer or a string literal
 *     must terminate with null char, 0x00
 * parameter location: destination position of first characer of message
 * parameter width: width of message for padding with blanks
 *     positive: pad with spaces to right
 *     negative: pad with spaces to left
 *     0: don't pad
 */
void writeToDisplay( const char *message, unsigned char location, char width )
{

	char* pos = (char *) message;
	char* pos2 = (char *) message;
	char i = 0, currentWidth = 0;

	if( width < 0 )
	{
		width = 0 - width; // make positive

		while( *( pos2++ ) != 0 )
			currentWidth++;

		for( i = 0; i < width - currentWidth; i++ )
			nextDisplay[location + i] = ' ';

		while( *pos != 0 )
			nextDisplay[location + i++] = *( pos++ );
	}
	else
	{
		while( *pos != 0 )
		{
			nextDisplay[location + i++] = *( pos++ );
			currentWidth++;
		}

		if( width > 0 )
		{
			for( i = 0; i < width - currentWidth; i++ )
			{
				nextDisplay[location + currentWidth + i] = ' ';
			}
		}
	}
}

/* updateMenu
 * Updates the menu by using detected button presses to switch between states
 */
void updateMenu( void )
{
	switch( menuState )
	{
		case MENU_DEBUG:
			menuDebug( );
			break;

		case MENU_HOME_BASIC:
			menuHomeBasic( );
			break;

		case MENU_HOME_DETAIL:
			menuHomeDetail( );
			break;

		case MENU_ALARM:
			menuAlarm( );
			break;

		case MENU_MAIN_1:
			menuMain1( );
			break;

		case MENU_MAIN_2:
			menuMain2( );
			break;

		case MENU_MAIN_3:
			menuMain3( );
			break;

		case MENU_MAIN_4:
			menuMain4( );
			break;

		case MENU_MAIN_5:
			menuMain5( );
			break;

		case MENU_MAIN_6:
			menuMain6( );
			break;

		case MENU_ALARM_1:
			menuAlarm1( );
			break;

		case MENU_ALARM_2:
			menuAlarm2( );
			break;

		case MENU_ALARM_3:
			menuAlarm3( );
			break;

		case MENU_ALARM_4:
			menuAlarm4( );
			break;

		case MENU_ALARM_5:
			menuAlarm5( );
			break;

			//        case MENU_ALARM_6:
			//            menuAlarm6();
			//            break;
			//
			//        case MENU_ALARM_7:
			//            menuAlarm7();
			//            break;

		case MENU_ABOUT:
			menuAbout( );
			break;

		case MENU_ADMIN_LOGIN:
			menuAdminLogin( );
			break;

		case MENU_ADMIN_1:
			menuAdmin1( );
			break;

		case MENU_ADMIN_2:
			menuAdmin2( );
			break;

		case MENU_ADMIN_3:
			menuAdmin3( );
			break;

		case MENU_ADMIN_4:
			menuAdmin4( );
			break;

		case MENU_ADMIN_5:
			menuAdmin5( );
			break;

		case MENU_ADMIN_6:
			menuAdmin6( );
			break;

		case MENU_SET_TIME:
			menuSetTime( );
			break;

		case MENU_SET_POWER:
			menuSetPower( );
			break;

		case MENU_SET_RESET_TIME:
			menuSetResetTime( );
			break;

		case MENU_EMERGENCY_1:
			menuEmergency1( );
			break;

		case MENU_EMERGENCY_2:
			menuEmergency2( );
			break;

		case MENU_EMERGENCY_3:
			menuEmergency3( );
			break;

		case MENU_EMERGENCY_4:
			menuEmergency4( );
			break;

		case MENU_MODULES:
			menuModules( );
			break;

		case MENU_SET_RELAY:
			menuSetRelay( );
			break;

		case MENU_STATISTICS:
			menuStatistics( );
			break;

		case MENU_POWERFAILTIMES:
			menuPowerFailTimes( );
			break;

		case MENU_BAD_DATE:
			menuBadDate( );
			break;

		case MENU_SHUT_OFF_WARNING:
			menuShutOffWarning( );
			break;

		case MENU_PASSWORD_CHANGE:
			menuPasswordChange( );
			break;

		case MENU_POWER_RESET:
			menuPowerReset( );
			break;

		default: // invalid menu var
			menuError( );
	}
}

/* Processes button presses. Changes menu state to the provided state for
 * the button which has been pushed. Returns the number of the button
 * which has been pressed (0-3) or -1 if no button has been pressed */
char menuButtonRead( char menu1, char menu2, char menu3, char menu4 )
{
	if( menu1 == 0 ) menu1 = menuState;
	if( menu2 == 0 ) menu2 = menuState;
	if( menu3 == 0 ) menu3 = menuState;
	if( menu4 == 0 ) menu4 = menuState;

	if( button0Flag == 1 )
	{
		menuState = menu1;
		button0Flag++;
		return 0;
	}
	else if( button1Flag == 1 )
	{
		menuState = menu2;
		button1Flag++;
		return 1;
	}
	else if( button2Flag == 1 )
	{
		menuState = menu3;
		button2Flag++;
		return 2;
	}
	else if( button3Flag == 1 )
	{
		menuState = menu4;
		button3Flag++;
		return 3;
	}
	return -1;
}

void menuError( void )
{
	menuButtonRead( MENU_HOME_BASIC, MENU_HOME_BASIC, MENU_HOME_BASIC, MENU_HOME_BASIC );

	int menuStateTemp;
	char menuStateBuf[BUF_SIZE_INT ];

	menuStateTemp = menuState;
	itoa( menuStateBuf, menuState, 10 );


	writeToDisplay( "ERROR 404:", 0, 20 );
	writeToDisplay( "Menu not found", 20, 57 );
	writeToDisplay( menuStateBuf, 77, -3 );
}

void menuDebug( void )
{

	menuButtonRead( 0, 0, 0, MENU_HOME_BASIC );

	//    writeClockStrings();
	//    writeSndBufToDisplay();
	writeToDisplay( "Debug Menu", 0, 20 );
	//writeToDisplay("  ", 8, 0);
	//writeToDisplay(itoa(buffer, timeSecond, 10), 0, 2);
	//writeToDisplay(ltoa(buffer, powerUsedMW, 10), 20, 11);
	//writeToDisplay(ltoa(buffer, powerUsed, 10), 40, 0);
	//writeToDisplay(ltoa(buffer, currentLoad, 10), 60, 0);

}

void menuHomeBasic( void )
{

	switch( menuButtonRead( MENU_MAIN_1, 0, 0, MENU_HOME_DETAIL ) )
	{
		case 0:
			com_command_readRemoteAlarm( );
			break;
		case 1:
		case 2:
			homeBasicAlternate++;
	}

	if( !BACKLIGHT )
	{
		homeBasicAlternate = ( timeSecond / 10 ) % 2;
	}

	readTime( );
	writeClockStrings( );

	if( homeBasicAlternate > 1 )
		homeBasicAlternate = 0;

	/* Normal, no seconds displayed */
#ifndef SECONDS_CLOCK
	writeToDisplay( "  ", 0, 0 );
	writeToDisplay( clockStr, 2, 5 );
	writeToDisplay( "   ", 7, 0 );
	writeToDisplay( calendarStr, 10, 0 );
	writeToDisplay( "  ", 18, 0 );
#else
	/* Debug, with seconds displayed */
	char specialClockStr[21];
	specialClockStr[0] = ' ';
	specialClockStr[1] = ( timeHour / 10 ) + 0x30;
	specialClockStr[2] = ( timeHour % 10 ) + 0x30;
	specialClockStr[3] = ':';
	specialClockStr[4] = ( timeMinute / 10 ) + 0x30;
	specialClockStr[5] = ( timeMinute % 10 ) + 0x30;
	specialClockStr[6] = ':';
	specialClockStr[7] = ( timeSecond / 10 ) + 0x30;
	specialClockStr[8] = ( timeSecond % 10 ) + 0x30;
	specialClockStr[9] = ' ';
	specialClockStr[10] = ' ';
	specialClockStr[11] = ( timeDay / 10 ) + 0x30;
	specialClockStr[12] = ( timeDay % 10 ) + 0x30;
	specialClockStr[13] = '/';
	specialClockStr[14] = ( timeMonth / 10 ) + 0x30;
	specialClockStr[15] = ( timeMonth % 10 ) + 0x30;
	specialClockStr[16] = '/';
	specialClockStr[17] = ( timeYear / 10 ) + 0x30;
	specialClockStr[18] = ( timeYear % 10 ) + 0x30;
	specialClockStr[19] = ' ';
	specialClockStr[20] = 0;
	writeToDisplay( specialClockStr, 0, 0 );
	/* End debug clock */
#endif

	if( !homeBasicAlternate )
	{
		char percentRemBuf[BUF_SIZE_INT];

		itoa( percentRemBuf, percentRem, 10 );

		writeToDisplay( "Remaining:", 20, 15 );
		writeToDisplay( percentRemBuf, 35, -3 );
		writeToDisplay( "% ", 38, 0 );
		writeBarGraph( );
		writeToDisplay( barGraph, 40, 0 );
	}
	else
	{

		char currentLoadBuf[BUF_SIZE_LONG];

		ultoa( currentLoadBuf, powerLoad, 10 );

		writeToDisplay( "Current Load:", 20, 0 );
		//        writeToDisplay(itoa(buffer1, currentLoad, 10), 33, -5);
		writeToDisplay( currentLoadBuf, 33, -5 );
		writeToDisplay( " W", 38, 0 );

		writeToDisplay( "Time left:", 40, 12 );
		writeToDisplay( timeRemainingString, 52, 0 );
	}

	//writeToDisplay(barGraph, 40, 0);
	writeToDisplay( "Menu   Swap   Detail", 60, 0 );

	if( !enablePeriodicUpdate )
	{
		enablePeriodicUpdate = 1;
	}
}

void menuHomeDetail( void )
{
	switch( menuButtonRead( MENU_MAIN_1, 0, 0, MENU_HOME_BASIC ) )
	{
		case 0:
			com_command_readRemoteAlarm( );
	}

	readTime( );
	writeClockStrings( );

	int remainingPower = energyAllocated - energyUsed;
	if( remainingPower < 0 )
	{
		remainingPower = 0;
	}

	writeToDisplay( "Left: ", 0, 0 );

	char remainingPowerBuf[BUF_SIZE_INT];
	char powerAllocatedBuf[BUF_SIZE_LONG];
	char percentRemBuf[BUF_SIZE_INT];
	char currentLoadBuf[BUF_SIZE_LONG];

	itoa( remainingPowerBuf, remainingPower, 10 );
	ltoa( powerAllocatedBuf, energyAllocated, 10 );
	itoa( percentRemBuf, percentRem, 10 );
	ultoa( currentLoadBuf, powerLoad, 10 );


	writeToDisplay( remainingPowerBuf, 6, -5 );
	writeToDisplay( "/", 11, 0 );
	writeToDisplay( powerAllocatedBuf, 12, -5 );
	writeToDisplay( "Wh Remaining:     ", 17, 0 );
	writeToDisplay( percentRemBuf, 35, -3 );
	writeToDisplay( "% Load:       ", 38, 0 );
	writeToDisplay( currentLoadBuf, 52, -5 );
	writeToDisplay( " W Menu           Basic", 57, 0 );

	if( !enablePeriodicUpdate )
	{
		enablePeriodicUpdate = 1;
	}
}

void menuAlarm( void )
{
	switch( menuButtonRead( 0, 0, 0, 0 ) )
	{
		case 0:
		case 1:
		case 2:
		case 3:
			menuState = oldMenuState;
			if( activeAlarm == 1 )
				silenceAlarmOne = 1;
			if( activeAlarm == 2 )
				silenceAlarmTwo = 1;
			alarmToResume = activeAlarm = remainingSets = 0;
	}


	char percentRemBuf[BUF_SIZE_INT];

	itoa( percentRemBuf, percentRem, 10 );

	writeToDisplay( "Warning: Low power! Only ", 0, 0 );

	writeToDisplay( percentRemBuf, 25, -2 );
	writeToDisplay( "% remaining.                         Clear Alarm     ", 27, 0 );
	//    writeToDisplay("% remaining.   ", 27, 0);
	//    writeToDisplay(itoa(buffer1, timeSecond, 10), 40, -2);
	//    writeToDisplay(itoa(buffer2, alarmEnd, 10), 42, -2);
	//    writeToDisplay(itoa(buffer3, activeAlarm, 10), 44, -2);
	//    writeToDisplay("                  Clear Alarm     ", 46, 0);
}

void menuMain1( void )
{

	switch( menuButtonRead( MENU_HOME_BASIC, MENU_MAIN_6, MENU_MAIN_2, MENU_ALARM_1 ) )
	{
		case 1:
			com_command_readRemoteCBver( );
			break;
		case 2:
			com_command_readRemotePassword( );
	}

	writeToDisplay( "Main Menu     1 of 6* Alarm Options       Admin Menu", 0, 60 );
	writeToDisplay( softKeys0, 60, 0 );

	enablePeriodicUpdate = 0;

}

void menuMain2( void )
{
	switch( menuButtonRead( MENU_HOME_BASIC, MENU_MAIN_1, MENU_MAIN_3, MENU_ADMIN_LOGIN ) )
	{
		case 1:
			com_command_readRemoteAlarm( );
			break;
		case 2:
			//com_command_readRemoteHL( );
			break;
		case 3:
			pwLength = 0;
	}

	writeToDisplay( "Main Menu     2 of 6* Admin Menu          Modules ", 0, 60 );
	writeToDisplay( softKeys0, 60, 0 );
}

void menuMain3( void )
{

	switch( menuButtonRead( MENU_HOME_BASIC, MENU_MAIN_2, MENU_MAIN_4, MENU_MODULES ) )
	{
		case 1:
			com_command_readRemotePassword( );
			break;
		case 2:
			com_command_readRemoteStat( );
			break;
		case 3:
			tempIsHigh = isHigh;
			if( highAlloc == 0 )
			{
				highAlloc = energyAllocated;
				lowAlloc = ( energyAllocated * 3 ) / 4;
			}
	}

	writeToDisplay( "Main Menu     3 of 6* Modules             Statistics", 0, 60 );
	writeToDisplay( softKeys0, 60, 0 );
}

void menuMain4( void )
{
	switch( menuButtonRead( MENU_HOME_BASIC, MENU_MAIN_3, MENU_MAIN_5, MENU_STATISTICS ) )
	{
		case 1:
			//com_command_readRemoteHL( );
			break;
		case 2:
			com_command_readRemotePowerFailTimes( );
	}

	writeToDisplay( "Main Menu     4 of 6* Statistics          Last Power Failure", 0, 60 );
	writeToDisplay( softKeys0, 60, 0 );
}

void menuMain5( void )
{
	switch( menuButtonRead( MENU_HOME_BASIC, MENU_MAIN_4, MENU_MAIN_6, MENU_POWERFAILTIMES ) )
	{
		case 1:
			com_command_readRemoteStat( );
			break;
		case 2:
			com_command_readRemoteCBver( );
	}

	writeToDisplay( "Main Menu     5 of 6* Last Power Failure  About", 0, 60 );
	writeToDisplay( softKeys0, 60, 0 );
}

void menuMain6( void )
{
	switch( menuButtonRead( MENU_HOME_BASIC, MENU_MAIN_5, MENU_MAIN_1, MENU_ABOUT ) )
	{
		case 1:
			com_command_readRemotePowerFailTimes( );
			break;
		case 2:
			com_command_readRemoteAlarm( );
	}

	writeToDisplay( "Main Menu     6 of 6  Last Power Failure* About", 0, 60 );
	writeToDisplay( softKeys1, 60, 0 );
}

void menuAlarm1( void )
{

	switch( menuButtonRead( MENU_MAIN_1, MENU_ALARM_4, MENU_ALARM_2, 0 ) )
	{
		case 0:
			com_command_setRemoteAlarm( );
			break;
		case 3:
			audibleAlarm++;
	}

	writeToDisplay( "Alarm Options 1 of 3* Audible Alarm  ", 0, 0 );

	if( audibleAlarm == 1 )
	{
		writeToDisplay( "On ", 37, 0 );
		audibleAlarm = 1;
	}
	else
	{
		writeToDisplay( "Off", 37, 0 );
		audibleAlarm = 0;
	}

	writeToDisplay( "  Alarm 1 Power  ", 40, 0 );

	if( alarm1Enabled )
	{
		char alarmOnePowerBuf[BUF_SIZE_INT];

		itoa( alarmOnePowerBuf, alarm1Energy, 10 );

		writeToDisplay( alarmOnePowerBuf, 57, -2 );
		writeToDisplay( "%", 59, 0 );
	}
	else
		writeToDisplay( "Off", 57, 0 );

	writeToDisplay( softKeys0, 60, 0 );
}

void menuAlarm2( void )
{

	switch( menuButtonRead( MENU_MAIN_1, MENU_ALARM_1, MENU_ALARM_4, MENU_ALARM_3 ) )
	{
		case 0:
			com_command_setRemoteAlarm( );
			break;
		case 3:
			tempPercent = alarm1Energy;
	}

	writeToDisplay( "Alarm Options 2 of 3* Alarm 1 Power  ", 0, 0 );

	if( alarm1Enabled )
	{
		char alarmOnePowerBuf[ BUF_SIZE_LONG];

		itoa( alarmOnePowerBuf, alarm1Energy, 10 );

		writeToDisplay( alarmOnePowerBuf, 37, -2 );
		writeToDisplay( "%", 39, 0 );
	}
	else
		writeToDisplay( "Off", 37, 0 );

	writeToDisplay( "  Alarm 2 Power  ", 40, 0 );

	if( alarm2Enabled )
	{
		char alarmTwoPowerBuf[ BUF_SIZE_INT];

		itoa( alarmTwoPowerBuf, alarm2Energy, 10 );

		writeToDisplay( alarmTwoPowerBuf, 57, -2 );
		writeToDisplay( "%", 59, 0 );
	}
	else
		writeToDisplay( "Off", 57, 0 );

	writeToDisplay( softKeys0, 60, 0 );
}

void menuAlarm3( void )
{

	switch( menuButtonRead( MENU_ALARM_2, 0, 0, MENU_ALARM_2 ) )
	{
		case 1:
			if( !alarm1Enabled )
				alarm1Enabled = 1;
			else
			{
				if( tempPercent < 20 )
					tempPercent++;
				else if( tempPercent < 95 )
					tempPercent += 5;
			}
			break;

		case 2:
			if( alarm1Enabled )
			{
				if( tempPercent > 20 )
					tempPercent -= 5;
				else if( tempPercent > 1 )
					tempPercent--;
				else if( tempPercent <= 1 )
					alarm1Enabled = 0;
			}
			break;

		case 3:
			alarm1Energy = tempPercent;
			alarmOneHit = 0;
			com_command_setRemoteAlarm( );
	}

	writeToDisplay( "Alarm Options 2 of 3* Alarm 1 Power ", 0, 0 );

	writeToDisplay( rightArrow, 36, 0 );

	if( alarm1Enabled )
	{
		char tempPercentBuf[BUF_SIZE_INT];

		itoa( tempPercentBuf, tempPercent, 10 );

		writeToDisplay( tempPercentBuf, 37, -2 );
		writeToDisplay( "%", 39, 0 );
	}
	else
		writeToDisplay( "Off", 37, 0 );

	writeToDisplay( "  Alarm 2 Power  ", 40, 0 );

	if( alarm2Enabled )
	{
		char alarmTwoPowerBuf[ BUF_SIZE_INT];

		itoa( alarmTwoPowerBuf, alarm2Energy, 10 );

		writeToDisplay( alarmTwoPowerBuf, 57, -2 );
		writeToDisplay( "%", 59, 0 );
	}
	else
		writeToDisplay( "Off", 57, 0 );

	writeToDisplay( softKeys2, 60, 0 );
}

void menuAlarm4( void )
{

	switch( menuButtonRead( MENU_MAIN_1, MENU_ALARM_2, MENU_ALARM_1, MENU_ALARM_5 ) )
	{
		case 0:
			com_command_setRemoteAlarm( );
			break;
		case 3:
			tempPercent = alarm2Energy;
	}

	writeToDisplay( "Alarm Options 3 of 3  Alarm 1 Power  ", 0, 0 );

	if( alarm1Enabled )
	{
		char alarmOnePowerBuf[ BUF_SIZE_INT];

		itoa( alarmOnePowerBuf, alarm1Energy, 10 );

		writeToDisplay( alarmOnePowerBuf, 37, -2 );
		writeToDisplay( "%", 39, 0 );
	}
	else
		writeToDisplay( "Off", 37, 0 );

	writeToDisplay( "* Alarm 2 Power  ", 40, 0 );

	if( alarm2Enabled )
	{
		char alarmTwoPowerBuf[ BUF_SIZE_INT];

		itoa( alarmTwoPowerBuf, alarm2Energy, 10 );

		writeToDisplay( alarmTwoPowerBuf, 57, -2 );
		writeToDisplay( "%", 59, 0 );
	}
	else
		writeToDisplay( "Off", 57, 0 );

	writeToDisplay( softKeys1, 60, 0 );
}

void menuAlarm5( void )
{

	switch( menuButtonRead( MENU_ALARM_4, 0, 0, MENU_ALARM_4 ) )
	{
		case 1:
			if( !alarm2Enabled )
				alarm2Enabled = 1;
			else
			{
				if( tempPercent < 20 )
					tempPercent++;
				else if( tempPercent < 95 )
					tempPercent += 5;
			}
			break;

		case 2:
			if( alarm2Enabled )
			{
				if( tempPercent > 20 )
					tempPercent -= 5;
				else if( tempPercent > 1 )
					tempPercent--;
				else if( tempPercent <= 1 )
					alarm2Enabled = 0;
			}
			break;

		case 3:
			alarm2Energy = tempPercent;
			alarmTwoHit = 0;
			com_command_setRemoteAlarm( );
	}

	writeToDisplay( "Alarm Options 3 of 3  Alarm 1 Power  ", 0, 0 );

	if( alarm1Enabled )
	{
		char alarmOnePowerBuf[BUF_SIZE_INT];

		itoa( alarmOnePowerBuf, alarm1Energy, 10 );

		writeToDisplay( alarmOnePowerBuf, 37, -2 );
		writeToDisplay( "%", 39, 0 );
	}
	else
		writeToDisplay( "Off", 37, 0 );

	writeToDisplay( "* Alarm 2 Power ", 40, 0 );
	writeToDisplay( rightArrow, 56, 0 );

	if( alarm2Enabled )
	{
		char tempPercentBuf[ BUF_SIZE_INT];

		itoa( tempPercentBuf, tempPercent, 10 );

		writeToDisplay( tempPercentBuf, 57, -2 );
		writeToDisplay( "%", 59, 0 );
	}
	else
		writeToDisplay( "Off", 57, 0 );

	writeToDisplay( softKeys2, 60, 0 );
}

void menuStatistics( void )
{

	char previousDayUsedBuf[BUF_SIZE_LONG];
	char totalUsedBuf[BUF_SIZE_LONG];

	menuButtonRead( MENU_MAIN_4, MENU_MAIN_4, MENU_MAIN_4, MENU_MAIN_4 );

	ltoa( previousDayUsedBuf, previousDayUsed, 10 );
	ltoa( totalUsedBuf, totalUsed, 10 );


	writeToDisplay( "Yesterday's usage:  ", 0, 0 );
	writeToDisplay( previousDayUsedBuf, 20, -17 );

	writeToDisplay( " WhTotal usage:", 37, 23 );
	writeToDisplay( totalUsedBuf, 60, -16 );
	writeToDisplay( " kWh", 76, 0 );
}

void menuPowerFailTimes( void )
{
	menuButtonRead( MENU_MAIN_5, MENU_MAIN_5, MENU_MAIN_5, MENU_MAIN_5 );

	writeToDisplay( "Last Power Failure  ", 0, 0 );
	writeToDisplay( "Lost     ", 20, 0 );
	writeToDisplay( powerDownTime, 29, 0 );
	writeToDisplay( "Restored ", 40, 0 );
	writeToDisplay( powerUpTime, 49, 0 );
	writeToDisplay( "Back", 60, 20 );
}

void menuAbout( void )
{
	menuButtonRead( MENU_MAIN_6, MENU_MAIN_6, MENU_MAIN_6, MENU_MAIN_6 );

	writeToDisplay( "Power Code: ", 0, 0 );
	writeToDisplay( powerBoxCodeVersionString, 12, 8 );
	writeToDisplay( "Disp Code:  ", 20, 0 );
	writeToDisplay( DISPLAY_BOX_CODE_VERSION, 32, 8 );
	writeToDisplay( "  Messiah College      Collaboratory    ", 40, 0 );
}

void menuAdminLogin( void )
{
	if( pwLength != 6 )
	{
		switch( menuButtonRead( 0, 0, 0, 0 ) )
		{
			case 0:
				passwordInput[pwLength++] = '1';
				break;
			case 1:
				passwordInput[pwLength++] = '2';
				break;
			case 2:
				passwordInput[pwLength++] = '3';
				break;
			case 3:
				passwordInput[pwLength++] = '4';
		}
	}
	else if( passwordInput[0] == passwordSet[0] &&
			 passwordInput[1] == passwordSet[1] &&
			 passwordInput[2] == passwordSet[2] &&
			 passwordInput[3] == passwordSet[3] &&
			 passwordInput[4] == passwordSet[4] &&
			 passwordInput[5] == passwordSet[5] )
	{
		menuState = MENU_ADMIN_1;
		com_command_readRemoteTime( );
		return;
	}
	else
	{
		menuState = MENU_HOME_BASIC;
		return;
	}

	writeToDisplay( "Administrator Login", 0, 44 );

	switch( pwLength )
	{
		case 0:
			writeToDisplay( "_ _ _ _ _ _", 44, 0 );
			break;

		case 1:
			writeToDisplay( "* _ _ _ _ _", 44, 0 );
			break;

		case 2:
			writeToDisplay( "* * _ _ _ _", 44, 0 );
			break;

		case 3:
			writeToDisplay( "* * * _ _ _", 44, 0 );
			break;

		case 4:
			writeToDisplay( "* * * * _ _", 44, 0 );
			break;

		case 5:
			writeToDisplay( "* * * * * _", 44, 0 );
			break;
	}

	writeToDisplay( "     1     2      3     4", 55, 0 );
}

void menuAdmin1( void )
{

	switch( menuButtonRead( MENU_MAIN_2, MENU_ADMIN_6, MENU_ADMIN_2, MENU_SET_TIME ) )
	{
		case 1:
			com_command_readRemoteRelay( );
			break;
		case 2:
			com_command_readRemoteEnergyAllocation( );
			break;
		case 3:
			timeSetPos = 1;

			// set temporary time variables to what's in the RTCC
			readTime( );
			tempHour = timeHour;
			tempMin = timeMinute;
			tempMonth = timeMonth;
			tempDay = timeDay;
			tempYear = timeYear;
	}

	writeToDisplay( "Admin Menu    1 of 6* Set Time/Date       Set Power Alloc", 0, 60 );
	writeToDisplay( softKeys0, 60, 0 );
}

void menuAdmin2( void )
{

	switch( menuButtonRead( MENU_MAIN_2, MENU_ADMIN_1, MENU_ADMIN_3, MENU_SET_POWER ) )
	{
		case 1:
			com_command_readRemoteTime( );
			break;
		case 2:
			com_command_readRemoteEnergy( );
			break;
		case 3:
			if( !highAlloc )
				highAlloc = energyAllocated;
			tempAlloc = highAlloc;
	}

	writeToDisplay( "Admin Menu    2 of 6* Set Power Alloc     Emergency Options", 0, 60 );
	writeToDisplay( softKeys0, 60, 0 );
}

void menuAdmin3( void )
{
	switch( menuButtonRead( MENU_MAIN_2, MENU_ADMIN_2, MENU_ADMIN_4, MENU_EMERGENCY_1 ) )
	{
		case 1:
			com_command_readRemoteEnergyAllocation( );
	}

	writeToDisplay( "Admin Menu    3 of 6* Emergency Options   Change Password", 0, 60 );
	writeToDisplay( softKeys0, 60, 0 );
}

void menuAdmin4( void )
{

	switch( menuButtonRead( MENU_MAIN_2, MENU_ADMIN_3, MENU_ADMIN_5, MENU_PASSWORD_CHANGE ) )
	{
		case 1:
			com_command_readRemoteEnergy( );
			break;
		case 2:
			com_command_readRemoteResetTime( );
			break;
		case 3:
			pwLength = 0;
			newPassword[0] = newPassword[2] = newPassword[4] = newPassword[6] =
					newPassword[8] = newPassword[10] = '_';
			newPassword[1] = newPassword[3] = newPassword[5] = newPassword[7] =
					newPassword[9] = ' ';
			newPassword[11] = 0;
	}

	writeToDisplay( "Admin Menu    4 of 6* Change Password     Change Reset Time", 0, 60 );
	writeToDisplay( softKeys0, 60, 0 );
}

void menuAdmin5( void )
{

	switch( menuButtonRead( MENU_MAIN_2, MENU_ADMIN_4, MENU_ADMIN_6, MENU_SET_RESET_TIME ) )
	{
		case 2:
			com_command_readRemoteRelay( );
			break;
		case 3:
			tempResetHour = resetTimeHour;
			tempResetMinute = resetTimeMinute;
	}

	writeToDisplay( "Admin Menu    5 of 6* Change Reset Time   Relay Control", 0, 60 );
	writeToDisplay( softKeys0, 60, 0 );
}

void menuAdmin6( void )
{

	switch( menuButtonRead( MENU_MAIN_2, MENU_ADMIN_5, MENU_ADMIN_1, MENU_SET_RELAY ) )
	{
		case 1:
			com_command_readRemoteResetTime( );
			break;
		case 2:
			com_command_readRemoteTime( );
			break;
		case 3:
			tempRelayActive = relayActive;
	}

	writeToDisplay( "Admin Menu    6 of 6  Change Reset Time * Relay Control", 0, 60 );
	writeToDisplay( softKeys1, 60, 0 );
}

void menuSetTime( void )
{

	switch( menuButtonRead( 0, 0, 0, 0 ) )
	{
		case 0:
			timeSetPos--;

			if( !timeSetPos )
			{
				if( isBooting )
					timeSetPos = 1;
				else
					menuState = MENU_ADMIN_1;
			}
			break;
		case 1:
			switch( timeSetPos )
			{
				case 1:
					tempHour = ( tempHour + 1 ) % 24;
					break;

				case 2:
					tempMin = ( tempMin + 1 ) % 60;
					break;

				case 3:
					tempDay--;
					tempDay = ( tempDay + 1 ) % 31;
					tempDay++;
					break;

				case 4:
					tempMonth--;
					tempMonth = ( tempMonth + 1 ) % 12;
					tempMonth++;
					break;

				case 5:
					tempYear++;
					if( tempYear > 99 )
						tempYear = 14;
			}
			break;
		case 2:
			switch( timeSetPos )
			{
				case 1:
					tempHour--;
					if( tempHour < 0 )
					{
						tempHour = 23;
					}
					break;

				case 2:
					tempMin--;
					if( tempMin < 0 )
					{
						tempMin = 59;
					}
					break;

				case 3:
					tempDay--;
					if( tempDay < 1 )
					{
						tempDay = 31;
					}
					break;

				case 4:
					tempMonth--;
					if( tempMonth < 1 )
					{
						tempMonth = 12;
					}
					break;

				case 5:
					tempYear--;
					if( tempYear < 14 )
					{
						tempYear = 99;
					}
			}
			break;

		case 3:
			timeSetPos++;
			if( timeSetPos == 6 )
			{
				if( writeTime( tempYear, tempMonth, tempDay, tempHour, tempMin, 0 ) == 1 )
				{
					com_command_setRemoteTime( );
					//                    lastUpdateSecond = timeSecond; // correct periodic update timing
					if( isBooting )
					{
						menuState = MENU_POWER_RESET;
						//isBooting = 0;
						com_command_readRemoteResetTime( );
					}
					else
					{
						menuState = MENU_ADMIN_1;
					}
				}
				else
				{
					menuState = MENU_BAD_DATE;
				}
			}
	}
	// timeSetPos indicates which field is currently selected
	// 1 - hours
	// 2 - minutes
	// 3 - month    (may want to switch to European date format)
	// 4 - day
	// 5 - year

	writeTempClockStrings( );

	writeToDisplay( "Set Time/Date", 0, 25 );
	writeToDisplay( tempClockStr, 25, 18 );
	writeToDisplay( tempCalStr, 43, 17 );

	if( timeSetPos != 5 )
	{
		writeToDisplay( "Back  Up  Down  Next", 60, 0 );
	}
	else
	{
		writeToDisplay( softKeys2, 60, 0 );
	}
	/*
	writeToDisplay(tempClockStr, 25, 0);
	writeToDisplay("         ", 34, 0);
	writeToDisplay(tempCalStr, 43, 0);

	if (timeSetPos != 5) {
	writeToDisplay("   Back  Up  Down  Next", 57, 0);
	} else {
	writeToDisplay("   Back  Up  Down  Save", 57, 0);
	}
	 */
	// when done use writeTime(newYear, newMonth, newDay, newHour, newMinute)
	// seconds, weekday are zeroed...
}

void menuSetPower( void )
{

	switch( menuButtonRead( MENU_ADMIN_2, 0, 0, 0 ) )
	{
		case 1:
			if( tempAlloc < 5 )
				tempAlloc++;
			else if( tempAlloc < 50 )
				tempAlloc += 5;
			else if( tempAlloc < 5000 )
				tempAlloc += 50;
			else if( tempAlloc < 10000 )
				tempAlloc += 100;
			else if( tempAlloc < 31999 )
				tempAlloc += 250;
			break;

		case 2:
			if( tempAlloc > 10000 )
				tempAlloc -= 250;
			else if( tempAlloc > 5000 )
				tempAlloc -= 100;
			else if( tempAlloc > 50 )
				tempAlloc -= 50;
			else if( tempAlloc > 5 )
				tempAlloc -= 5;
			else if( tempAlloc > 0 )
				tempAlloc--;

			// to fix unexpected boundary conditions
			if( tempAlloc < 0 || tempAlloc > 32000 )
				tempAlloc = 0;
			break;

		case 3:
			// high/low
			isHigh = 0xFF;

			if( energyUsed < tempAlloc )
			{
				menuState = MENU_ADMIN_2;
				energyAllocated = tempAlloc;
				highAlloc = tempAlloc;
				lowAlloc = ( highAlloc * 3 ) / 4;
			}
			else
			{
				menuState = MENU_SHUT_OFF_WARNING;
				oldPowerMenuState = MENU_SET_POWER;
			}
			isHigh = 0xFF;
			//com_command_setRemoteHL( );
			com_command_setRemoteEnergyAllocation( );
	}

	char tempAllocBuf[BUF_SIZE_INT];

	itoa( tempAllocBuf, tempAlloc, 10 );


	writeToDisplay( "Set Power Allocation  ", 0, 0 );
	writeToDisplay( tempAllocBuf, 22, -5 );
	//    writeToDisplay(" Wh per day                      Back  Up  Down  Save", 27, 0);
	writeToDisplay( " Wh per day", 27, 33 );
	writeToDisplay( softKeys2, 60, 0 );
}

void menuSetResetTime( void )
{

	switch( menuButtonRead( MENU_ADMIN_5, 0, 0, MENU_ADMIN_5 ) )
	{
		case 1:
			if( !tempResetMinute )
				tempResetMinute = 30;
			else
			{
				tempResetMinute = 0;
				tempResetHour = ( tempResetHour + 1 ) % 24;
			}
			break;

		case 2:
			if( tempResetMinute == 30 )
				tempResetMinute = 0;
			else
			{
				tempResetMinute = 30;
				tempResetHour--;

				if( tempResetHour > 23 )
					tempResetHour = 23;
			}
			break;

		case 3:
			resetTimeHour = tempResetHour;
			resetTimeMinute = tempResetMinute;

			char rth[20];
			char rtm[20];
			itoa( rth, tempResetHour, 10 );
			itoa( rtm, tempResetMinute, 10 );

			writeToDisplay( "Resetting Reset Time", 0, 20 );
			writeToDisplay( rth, 20, 20 );
			writeToDisplay( rtm, 40, 20 );
			writeToDisplay( " ", 60, 20 );

			delayMS( 2000 );

			com_command_setRemoteResetTime( );
	}

	tempResetTimeString[0] = ( tempResetHour / 10 ) + 0x30;
	tempResetTimeString[1] = ( tempResetHour % 10 ) + 0x30;
	tempResetTimeString[2] = ':';
	char rtm[20];

	itoa( rtm, tempResetMinute, 10 );
	if( rtm[1] == CHAR_NULL )
	{
		tempResetTimeString[3] = '0';
		tempResetTimeString[4] = rtm[0];
	}
	else
	{
		tempResetTimeString[3] = rtm[0];
		tempResetTimeString[4] = rtm[1];
	}
	tempResetTimeString[5] = CHAR_NULL;


	//    if( !tempResetMinute )
	//	tempResetTimeString[3] = '0';
	//    else
	//	tempResetTimeString[3] = '3';
	//    tempResetTimeString[4] = '0';
	//    tempResetTimeString[5] = 0;

	writeToDisplay( "Set Reset Time", 0, 27 );
	writeToDisplay( tempResetTimeString, 27, 33 );
	writeToDisplay( softKeys2, 60, 0 );
}

void menuEmergency1( void )
{

	switch( menuButtonRead( MENU_ADMIN_3, MENU_EMERGENCY_2, MENU_EMERGENCY_2, MENU_EMERGENCY_3 ) )
	{
		case 3:
			emerButtonAllocTemp = emerButtonEnergyAllocate;
	}

	writeToDisplay( "Emergency Options   * Emer Button    ", 0, 0 );

	if( emerButtonEnable > 0 )
	{
		char emerButtonAllocBuf[BUF_SIZE_INT];

		itoa( emerButtonAllocBuf, emerButtonEnergyAllocate, 10 );

		writeToDisplay( emerButtonAllocBuf, 37, -3 );
		emerButtonEnable = 1;
	}
	else
	{
		writeToDisplay( "Off", 37, 0 );
		emerButtonEnable = 0;
	}

	char emerAllocNowBuf[ BUF_SIZE_INT];

	itoa( emerAllocNowBuf, emerAllocNow, 10 );

	writeToDisplay( "  Emer Alloc Now  ", 40, 0 );

	writeToDisplay( emerAllocNowBuf, 57, -3 );

	writeToDisplay( softKeys0, 60, 0 );
}

void menuEmergency2( void )
{
	menuButtonRead( MENU_ADMIN_3, MENU_EMERGENCY_1, MENU_EMERGENCY_1, MENU_EMERGENCY_4 );

	writeToDisplay( "Emergency Options     Emer Button    ", 0, 0 );

	if( emerButtonEnable == 1 )
	{
		char emerButtonAllocBuf[ BUF_SIZE_INT];

		itoa( emerButtonAllocBuf, emerButtonEnergyAllocate, 10 );

		writeToDisplay( emerButtonAllocBuf, 37, -3 );
		emerButtonEnable = 1;
	}
	else
	{
		writeToDisplay( "Off", 37, 0 );
		emerButtonEnable = 0;
	}

	char emerAllocNowBuf[ BUF_SIZE_INT];

	itoa( emerAllocNowBuf, emerAllocNow, 10 );

	writeToDisplay( "* Emer Alloc Now ", 40, 0 );
	writeToDisplay( emerAllocNowBuf, 57, -3 );

	writeToDisplay( softKeys0, 60, 0 );
}

void menuEmergency3( void )
{

	switch( menuButtonRead( MENU_EMERGENCY_1, 0, 0, MENU_EMERGENCY_1 ) )
	{
		case 1:
			if( !emerButtonEnable )
			{
				emerButtonEnable = 1;
				emerButtonAllocTemp = 50;
			}
			else
			{
				if( emerButtonAllocTemp < 950 )
					emerButtonAllocTemp += 50;
			}
			break;

		case 2:
			if( emerButtonEnable )
			{
				if( emerButtonAllocTemp < 100 )
				{
					emerButtonEnable = 0;
					emerButtonAllocTemp = 50;
				}
				else
					emerButtonAllocTemp -= 50;
			}
			break;

		case 3:
			emerButtonEnergyAllocate = emerButtonAllocTemp;
			emerAllocSend = 0;
			com_command_setRemoteEmergency( );
	}

	writeToDisplay( "Emergency Options   * Emer Button   ", 0, 0 );
	writeToDisplay( rightArrow, 36, 0 );

	if( emerButtonEnable > 0 )
	{
		char emerButtonAllocTempBuf[ BUF_SIZE_INT];

		itoa( emerButtonAllocTempBuf, emerButtonAllocTemp, 10 );


		writeToDisplay( emerButtonAllocTempBuf, 37, -3 );
		emerButtonEnable = 1;
	}
	else
	{
		writeToDisplay( "Off", 37, 0 );
		emerButtonEnable = 0;
	}

	char emerAllocNowBuf[ BUF_SIZE_INT];

	itoa( emerAllocNowBuf, emerAllocNow, 10 );

	writeToDisplay( "  Emer Alloc Now ", 40, 0 );

	writeToDisplay( emerAllocNowBuf, 57, -3 );

	writeToDisplay( softKeys2, 60, 0 );

}

void menuEmergency4( void )
{

	switch( menuButtonRead( MENU_EMERGENCY_2, 0, 0, MENU_EMERGENCY_2 ) )
	{
		case 1:
			if( emerAllocNow < 950 )
				emerAllocNow += 50;
			break;
		case 2:
			if( emerAllocNow >= 100 )
				emerAllocNow -= 50;
			break;
		case 3:
			emerAllocSend = emerAllocNow;
			com_command_setRemoteAllocationAdd( );
			emerAllocSend = 0;
	}

	writeToDisplay( "Emergency Options     Emer Button    ", 0, 0 );

	if( emerButtonEnable == 1 )
	{
		char emerButtonAllocBuf[ BUF_SIZE_INT];

		itoa( emerButtonAllocBuf, emerButtonEnergyAllocate, 10 );

		writeToDisplay( emerButtonAllocBuf, 37, -3 );
	}
	else
	{
		writeToDisplay( "Off", 37, 0 );
		emerButtonEnable = 0;
	}

	char emerAllocNowBuf[ BUF_SIZE_INT];

	itoa( emerAllocNowBuf, emerAllocNow, 10 );

	writeToDisplay( "* Emer Alloc Now", 40, 0 );
	writeToDisplay( rightArrow, 56, 0 );
	writeToDisplay( emerAllocNowBuf, 57, -3 );

	writeToDisplay( softKeys2, 60, 0 );
}

void menuModules( void )
{

	switch( menuButtonRead( MENU_MAIN_3, 0, 0, 0 ) )
	{
		case 1:
			tempIsHigh = 0xFF;
			break;
		case 2:
			tempIsHigh = 0;
			break;
		case 3:
			if( tempIsHigh )
				tempAlloc = highAlloc;
			else
				tempAlloc = lowAlloc;

			if( energyUsed < tempAlloc )
			{
				menuState = MENU_MAIN_3;
				isHigh = tempIsHigh;
				energyAllocated = tempAlloc;
			}
			else
			{
				menuState = MENU_SHUT_OFF_WARNING;
				oldPowerMenuState = MENU_MODULES;
			}
			//com_command_setRemoteHL( );
	}

	writeToDisplay( "Modules Placeholder    ", 0, 0 );
	if( tempIsHigh )
		writeToDisplay( rightArrow, 23, 0 );
	else
		writeToDisplay( " ", 23, 0 );
	writeToDisplay( "High               ", 24, 0 );
	if( tempIsHigh )
		writeToDisplay( " ", 43, 0 );
	else
		writeToDisplay( rightArrow, 43, 0 );
	writeToDisplay( "Low             Back  High  Low   OK", 44, 0 );
}

void menuSetRelay( void )
{

	switch( menuButtonRead( MENU_ADMIN_6, 0, 0, MENU_ADMIN_6 ) )
	{
		case 1:
			tempRelayActive = 0xFF;
			break;
		case 2:
			tempRelayActive = 0;
			break;
		case 3:
			relayActive = tempRelayActive;
			com_command_setRemoteRelay( );
	}

	writeToDisplay( "Relay Functionality", 0, 23 );

	if( tempRelayActive )
		writeToDisplay( rightArrow, 23, 0 );
	else
		writeToDisplay( " ", 23, 0 );

	writeToDisplay( "On", 24, 19 );

	if( tempRelayActive )
		writeToDisplay( " ", 43, 0 );
	else
		writeToDisplay( rightArrow, 43, 0 );

	writeToDisplay( "Off", 44, 16 );
	writeToDisplay( "Back   On   Off   OK", 60, 0 );

}

void menuBadDate( void )
{
	if( menuButtonRead( MENU_SET_TIME, MENU_SET_TIME, MENU_SET_TIME, MENU_SET_TIME ) != -1 )
		timeSetPos--;

	writeToDisplay( "Invalid date entered", 0, 60 );
	writeToDisplay( "Return", 60, 20 );
}

void menuShutOffWarning( void )
{

	switch( menuButtonRead( oldPowerMenuState, oldPowerMenuState, 0, 0 ) )
	{
		case 2:
		case 3:
			if( oldPowerMenuState == MENU_SET_POWER )
				menuState = MENU_ADMIN_2;
			else
			{
				menuState = MENU_MAIN_3;
				isHigh = tempIsHigh;

			}
			energyAllocated = tempAlloc;
			com_command_setRemoteEnergyAllocation( );
	}

	writeToDisplay( "Continuing will     cause power to shut off immediately!    Return      Continue", 0, 0 );
}

void menuPasswordChange( void )
{
	if( pwLength < 6 )
	{
		switch( menuButtonRead( 0, 0, 0, 0 ) )
		{
			case 0:
				newPassword[pwLength * 2] = '1';
				passwordInput[pwLength++] = '1';
				break;
			case 1:
				newPassword[pwLength * 2] = '2';
				passwordInput[pwLength++] = '2';
				break;
			case 2:
				newPassword[pwLength * 2] = '3';
				passwordInput[pwLength++] = '3';
				break;
			case 3:
				newPassword[pwLength * 2] = '4';
				passwordInput[pwLength++] = '4';
		}

	}
	else
	{
		switch( menuButtonRead( 0, 0, 0, MENU_ADMIN_4 ) )
		{
			case 0:
				pwLength = 0;
				//FIX DONE
				//stringCopy( "_ _ _ _ _ _", newPassword );
				newPassword[0] = '_';
				newPassword[1] = ' ';
				newPassword[2] = '_';
				newPassword[3] = ' ';
				newPassword[4] = '_';
				newPassword[5] = ' ';
				newPassword[6] = '_';
				newPassword[7] = ' ';
				newPassword[8] = '_';
				newPassword[9] = ' ';
				newPassword[10] = '_';
				newPassword[11] = CHAR_NULL;



				break;
			case 1:
			case 2:
				break;
			case 3:
				passwordSet[0] = passwordInput[0];
				passwordSet[1] = passwordInput[1];
				passwordSet[2] = passwordInput[2];
				passwordSet[3] = passwordInput[3];
				passwordSet[4] = passwordInput[4];
				passwordSet[5] = passwordInput[5];
				com_command_setRemotePassword( );
				pwLength = 0;
		}

	}

	writeToDisplay( "Enter new password:", 0, 44 );

	writeToDisplay( newPassword, 44, 16 );

	if( pwLength < 6 )
		writeToDisplay( "1     2      3     4", 60, 20 );
	else
		writeToDisplay( "Clear           Save", 60, 0 );
}

void menuPowerReset( void )
{

	switch( menuButtonRead( MENU_HOME_BASIC, 0, 0, MENU_HOME_BASIC ) )
	{
		case 3:
			com_command_doReset( ); // fall through on purpose
		case 0:
			isBooting = 0;
	}

	writeToDisplay( "Is it after the     reset time?", 0, 46 );

	char resetHourBuf[BUF_SIZE_INT];
	itoa( resetHourBuf, resetTimeHour, 10 );


	if( resetTimeHour < 10 )
	{
		writeToDisplay( " (", 46, 0 );
		writeToDisplay( resetHourBuf, 48, 0 );
	}
	else
	{
		writeToDisplay( "(", 46, 0 );
		writeToDisplay( resetHourBuf, 47, 0 );
	}

	writeToDisplay( ":", 49, 0 );
	if( resetTimeMinute )
	{
		writeToDisplay( "3", 50, 0 );
	}
	else
	{
		writeToDisplay( "0", 50, 0 );
	}
	writeToDisplay( "0)       No               Yes", 51, 0 );
}

void writeBarGraph( void )
{
	barGraph[20] = 0;
	unsigned char counter;
	unsigned char filled;

	filled = percentRem / 5;

	for( counter = 0; counter < 20; counter++ )
	{
		if( counter < filled )
			barGraph[counter] = 0xFF;
		else
			barGraph[counter] = '_';
	}

	if( percentRem < 5 && energyUsed < energyAllocated )
		barGraph[0] = '|';
}

void calcPercentRem( void )
{
	percentRem = ( energyAllocated - energyUsed ) * 100 / energyAllocated;

	if( ( percentRem < 0 ) || ( energyUsed > energyAllocated ) )
		percentRem = 0;

	else if( percentRem > 100 )
		percentRem = 100;
}

void calcTimeRemaining( void )
{
	// variables:
	// timeRemHour, timeRemMinute, timeRemSecond for actual time
	// timeRemUpdate for running average
	// timeRem1, timeRem2, timeRem3, timeRem4, timeRem5 to store values

	if( timeSecond != timeRemUpdate )
	{

		timeRemainingString[2] = timeRemainingString[5] = ':';
		timeRemainingString[8] = 0;

		timeRemUpdate = timeSecond;
		timeRem1 = timeRem2;
		timeRem2 = timeRem3;
		timeRem3 = timeRem4;
		timeRem4 = timeRem5;
		timeRem5 = powerLoad;

		unsigned long timeRemAvg = ( timeRem1 + timeRem2 + timeRem3 + timeRem4 + timeRem5 ) / 5;

		if( !timeRemAvg )
		{
			timeRemainingString[0] = timeRemainingString[3] = timeRemainingString[6] = '5';
			timeRemainingString[1] = timeRemainingString[4] = timeRemainingString[7] = '9';
		}
		else
		{
			long powerRemaining = energyAllocated - energyUsed;
			unsigned long timeRemaining = ( 3600 * powerRemaining ) / timeRemAvg;
			timeRemHour = timeRemaining / 3600;
			timeRemMinute = ( timeRemaining / 60 ) - ( 60 * timeRemHour );
			timeRemSecond = timeRemaining % 60;

			if( timeRemHour > 59 ) timeRemHour = 59;

			timeRemainingString[0] = timeRemHour / 10 + 0x30;
			timeRemainingString[1] = timeRemHour % 10 + 0x30;
			timeRemainingString[3] = timeRemMinute / 10 + 0x30;
			timeRemainingString[4] = timeRemMinute % 10 + 0x30;
			timeRemainingString[6] = timeRemSecond / 10 + 0x30;
			timeRemainingString[7] = timeRemSecond % 10 + 0x30;
		}
	}
}


/* Interrupts *****************************************************************/

/* Timer 1 Interrupt
 * Updates the display with changes made to the nextDisplay array
 * Updates currentDisplay array with what is now on the display
 * Only changes lines that are different between nextDisplay and currentDisplay
 */
void __attribute__( ( interrupt, no_auto_psv ) ) _T1Interrupt( void )
{

	unsigned char i, j, k;

	// loop through each line
	for( i = 0; i < NUM_LCD_LINES; i++ )
	{

		unsigned char lineOffset = i * NUM_LCD_WIDTH;

		// in each line check to see if current matches next
		unsigned char matches = 0xFF;

		for( j = 0; j < NUM_LCD_WIDTH; j++ )
		{

			if( currentDisplay[lineOffset + j] !=
			 nextDisplay[lineOffset + j] )
			{

				currentDisplay[lineOffset + j] =
						nextDisplay[lineOffset + j];
				matches = 0;
			}
		}

		if( ( !matches && currentDisplay[lineOffset] != 0 ) )
		{

			char tempArray[NUM_LCD_WIDTH + 1];
			tempArray[NUM_LCD_WIDTH] = 0;
			for( k = 0; k < NUM_LCD_WIDTH; k++ )
			{
				tempArray[k] = currentDisplay[lineOffset + k];
			}

			unsigned char index = 0;

			switch( i )
			{
				case 0:
					dspCommand( FIRST_LINE );
					while( tempArray[index] != 0 )
						dspWrite( tempArray[index++] );
					break;

				case 1:
					dspCommand( SECOND_LINE );
					while( tempArray[index] != 0 )
						dspWrite( tempArray[index++] );
					break;

				case 2:
					dspCommand( THIRD_LINE );
					while( tempArray[index] != 0 )
						dspWrite( tempArray[index++] );
					break;

				case 3:
					dspCommand( FOURTH_LINE );
					while( tempArray[index] != 0 )
						dspWrite( tempArray[index++] );

			}
		}
	}

	_T1IF = 0; // clear interrupt flag
}
