/****************
 INCLUDES
 only include the header files that are required
 ****************/
#include <stdlib.h>
#include "common.h"

#include "Communications.h"
#include "DisplayRTCC.h"
#include "DisplayMenuMacros.h"
#include "DisplayMain.h"
#include "DisplayMenu.h"

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

char passwordSet_global[6];

long previousDayUsed_global = 0;
long totalUsed_global = 0;

char relayMode_global;
char relayModeTemp_module;


const char softKeys_BackUpDownOK_module[] = "Back  Up  Down    OK";
const char softKeys_BackUpTopOK_module[] = "Back  Up  Top     OK";
const char softKeys_BackUpDownSave_module[] = "Back  Up  Down  Save";


bool audibleAlarm_global;
char alarm1PercentThreshold_global;
char alarm2PercentThreshold_global;

bool emerButtonEnable_global;

int resetTimeHour_global;
int resetTimeMinute_global;
int tempResetHour_global;
int tempResetMinute_global;


char oldPowerMenuState_module;


int emerButtonEnergyAllocate_global;
int emerButtonAllocTemp_module;
int emerAllocNow_global;
int emerAllocSend_global;

char powerDownTime_global[12];
char powerUpTime_global[12];

char powerBoxCodeVersionString_global[9];

char passwordInput_module[6];
char newPassword_module[12];
unsigned char pwLength_module;

char homeBasicAlternate_module; // 0 for % remaining, 1 for % load

char barGraph_module[21];

char tempResetTimeString_module[6];

unsigned char timeRemHour_module;
unsigned char timeRemMinute_module;
unsigned char timeRemSecond_module;
unsigned char timeRemUpdate_module;
unsigned long timeRem1_module;
unsigned long timeRem2_module;
unsigned long timeRem3_module;
unsigned long timeRem4_module;
unsigned long timeRem5_module;

char timeRemainingString_module[9];

char currentDisplay_module[NUM_LCD_LINES * NUM_LCD_WIDTH + 1];
char nextDisplay_module[NUM_LCD_LINES * NUM_LCD_WIDTH + 1];

unsigned char menuState_global;
unsigned char oldMenuState_global;
bool enablePeriodicUpdate_global;

char alarmActive_global;
char alarmToResume_global;
char rightArrow_module[2];
char tempPercent_global;
bool alarm1Silence_global;
bool alarm2Silence_global;
char alarmRemainingSets_global;

unsigned long powerLoad_global;
unsigned long energyUsed_global;
unsigned long energyAllocated_global;
unsigned long tempAlloc_module;

char percentRem_global;

unsigned char button0Flag_global;
unsigned char button1Flag_global;
unsigned char button2Flag_global;
unsigned char button3Flag_global;


char isBooting_global;

bool alarm1Hit_global;
bool alarm2Hit_global;



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
	__delay_ms( 2 );

	return;
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

	return;
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
	{
		dspWrite( *pos++ );
	}

	return;
}

/* nibble
 * sends half of a byte of data to the LCD, already on output lines to LCD
 */
void dspNibble( void )
{
	LCD_E = 1;

	// delay so the pin is held high for a minimum amount of time
	// 35us seems to be the lower limit - at 30 the display was showing artifacts
	// likely this should be increased for provide a safety margin
	// but keep in mind, this is 2x per byte sent to the display (4 bits at a time))
	__delay_us( 40 ); //enable pulse width >= 300ns

	LCD_E = 0; //Clock enable: falling edge

	return;
}

/* initDisplay
 * initializes the LCD for 4-bit operation
 */
void initDisplay( void )
{
	dspSetP1( 0 );

	__delay_ms( 100 ); //Wait >15 msec after power is applied
	dspSetP1( 3 ); //put 0x30 on the output port
	__delay_ms( 30 ); //must wait 5ms, busy flag not available

	dspNibble( ); //command 0x30 = Wake up
	__delay_ms( 10 ); //must wait 160us, busy flag not available
	dspNibble( ); //command 0x30 = Wake up #2
	__delay_ms( 10 ); //must wait 160us, busy flag not available
	dspNibble( ); //command 0x30 = Wake up #3
	__delay_ms( 10 ); //can check busy flag now instead of delay
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
	__delay_ms( 2 );
	dspCommand( FIRST_LINE ); //Return To First Line

	rightArrow_module[0] = 0x7E; // right arrow character for menus
	rightArrow_module[1] = 0x00;

	unsigned char i;

	if( BACKLIGHT_NORMAL == true )
	{
		BACKLIGHT = 1; // turn on backlight
	}

	// initialize starting arrays to 0
	for( i = 0; i < NUM_LCD_LINES * NUM_LCD_WIDTH; i++ )
	{
		currentDisplay_module[i] = ' ';
		nextDisplay_module[i] = ' ';
	}

	return;
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

	return;
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
		{
			currentWidth++;
		}

		for( i = 0; i < width - currentWidth; i++ )
		{
			nextDisplay_module[location + i] = ' ';
		}

		while( *pos != 0 )
		{
			nextDisplay_module[location + i++] = *( pos++ );
		}
	}
	else
	{
		while( *pos != 0 )
		{
			nextDisplay_module[location + i++] = *( pos++ );
			currentWidth++;
		}

		if( width > 0 )
		{
			for( i = 0; i < width - currentWidth; i++ )
			{
				nextDisplay_module[location + currentWidth + i] = ' ';
			}
		}
	}

	return;
}

/* updateMenu
 * Updates the menu by using detected button presses to switch between states
 */
void updateMenu( void )
{
	switch( menuState_global )
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

	return;
}

/* Processes button presses. Changes menu state to the provided state for
 * the button which has been pushed. Returns the number of the button
 * which has been pressed (0-3) or -1 if no button has been pressed */
char menuButtonRead( char menu1, char menu2, char menu3, char menu4 )
{
	char returnValue;

	returnValue = -1;

	if( menu1 == 0 )
	{
		menu1 = menuState_global;
	}

	if( menu2 == 0 )
	{
		menu2 = menuState_global;
	}

	if( menu3 == 0 )
	{
		menu3 = menuState_global;
	}

	if( menu4 == 0 )
	{
		menu4 = menuState_global;
	}

	if( button0Flag_global == 1 )
	{
		menuState_global = menu1;
		button0Flag_global++;
		returnValue = 0;
	}
	else if( button1Flag_global == 1 )
	{
		menuState_global = menu2;
		button1Flag_global++;
		returnValue = 1;
	}
	else if( button2Flag_global == 1 )
	{
		menuState_global = menu3;
		button2Flag_global++;
		returnValue = 2;
	}
	else if( button3Flag_global == 1 )
	{
		menuState_global = menu4;
		button3Flag_global++;
		returnValue = 3;
	}

	return returnValue;
}

void menuError( void )
{
	menuButtonRead( MENU_HOME_BASIC, MENU_HOME_BASIC, MENU_HOME_BASIC, MENU_HOME_BASIC );

	int menuStateTemp;
	char menuStateBuf[BUF_SIZE_INT ];

	menuStateTemp = menuState_global;
	itoa( menuStateBuf, menuState_global, 10 );


	writeToDisplay( "ERROR 404:", 0, 20 );
	writeToDisplay( "Menu not found", 20, 57 );
	writeToDisplay( menuStateBuf, 77, -3 );

	return;
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

	return;
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
			homeBasicAlternate_module++;
	}

	if( !BACKLIGHT )
	{
		homeBasicAlternate_module = ( timeSecond_global / 10 ) % 2;
	}

	rtccReadTime( );
	writeClockStrings( );

	if( homeBasicAlternate_module > 1 )
	{
		homeBasicAlternate_module = 0;
	}

	/* Normal, no seconds displayed */
#ifndef SECONDS_CLOCK
	writeToDisplay( "  ", 0, 0 );
	writeToDisplay( clockStr_global, 2, 5 );
	writeToDisplay( "   ", 7, 0 );
	writeToDisplay( calendarStr_global, 10, 0 );
	writeToDisplay( "  ", 18, 0 );
#else
	/* Debug, with seconds displayed */
	char specialClockStr[21];
	specialClockStr[0] = ' ';
	specialClockStr[1] = ( timeHour_global / 10 ) + 0x30;
	specialClockStr[2] = ( timeHour_global % 10 ) + 0x30;
	specialClockStr[3] = ':';
	specialClockStr[4] = ( timeMinute_global / 10 ) + 0x30;
	specialClockStr[5] = ( timeMinute_global % 10 ) + 0x30;
	specialClockStr[6] = ':';
	specialClockStr[7] = ( timeSecond_global / 10 ) + 0x30;
	specialClockStr[8] = ( timeSecond_global % 10 ) + 0x30;
	specialClockStr[9] = ' ';
	specialClockStr[10] = ' ';
	specialClockStr[11] = ( timeDay_global / 10 ) + 0x30;
	specialClockStr[12] = ( timeDay_global % 10 ) + 0x30;
	specialClockStr[13] = '/';
	specialClockStr[14] = ( timeMonth_global / 10 ) + 0x30;
	specialClockStr[15] = ( timeMonth_global % 10 ) + 0x30;
	specialClockStr[16] = '/';
	specialClockStr[17] = ( timeYear_global / 10 ) + 0x30;
	specialClockStr[18] = ( timeYear_global % 10 ) + 0x30;
	specialClockStr[19] = ' ';
	specialClockStr[20] = 0;
	writeToDisplay( specialClockStr, 0, 0 );
	/* End debug clock */
#endif

	if( !homeBasicAlternate_module )
	{
		char percentRemBuf[BUF_SIZE_INT];

		itoa( percentRemBuf, percentRem_global, 10 );

		writeToDisplay( "Remaining:", 20, 15 );
		writeToDisplay( percentRemBuf, 35, -3 );
		writeToDisplay( "% ", 38, 0 );
		writeBarGraph( );
		writeToDisplay( barGraph_module, 40, 0 );
	}
	else
	{
		char currentLoadBuf[BUF_SIZE_LONG];

		ultoa( currentLoadBuf, powerLoad_global, 10 );

		writeToDisplay( "Current Load:", 20, 0 );
		//        writeToDisplay(itoa(buffer1, currentLoad, 10), 33, -5);
		writeToDisplay( currentLoadBuf, 33, -5 );
		writeToDisplay( " W", 38, 0 );

		writeToDisplay( "Time left:", 40, 12 );
		writeToDisplay( timeRemainingString_module, 52, 0 );
	}

	//writeToDisplay(barGraph, 40, 0);
	writeToDisplay( "Menu   Swap   Detail", 60, 0 );

	// allow parameters to be updated
	enablePeriodicUpdate_global = true;

	return;
}

void menuHomeDetail( void )
{
	switch( menuButtonRead( MENU_MAIN_1, 0, 0, MENU_HOME_BASIC ) )
	{
		case 0:
			com_command_readRemoteAlarm( );
	}

	rtccReadTime( );
	writeClockStrings( );

	long remainingPower;

	// normal use the remaining power will never be < 0
	// but we allow it here to help catch if the meter is somehow bypassed
	// a negative number means someone is likely cheating
	remainingPower = energyAllocated_global - energyUsed_global;

	writeToDisplay( "Used: ", 0, 0 );

	char remainingPowerBuf[BUF_SIZE_INT];
	char powerAllocatedBuf[BUF_SIZE_LONG];
	char percentRemBuf[BUF_SIZE_INT];
	char currentLoadBuf[BUF_SIZE_LONG];

	ltoa( remainingPowerBuf, remainingPower, 10 );
	ltoa( powerAllocatedBuf, energyAllocated_global, 10 );
	itoa( percentRemBuf, percentRem_global, 10 );
	ultoa( currentLoadBuf, powerLoad_global, 10 );


	writeToDisplay( remainingPowerBuf, 5, -5 );
	writeToDisplay( "/", 11, 0 );
	writeToDisplay( powerAllocatedBuf, 12, -5 );
	writeToDisplay( "Wh Remaining:     ", 17, 0 );
	writeToDisplay( percentRemBuf, 35, -3 );
	writeToDisplay( "% Load:       ", 38, 0 );
	writeToDisplay( currentLoadBuf, 52, -5 );
	writeToDisplay( " W Menu           Basic", 57, 0 );

	if( enablePeriodicUpdate_global == false )
	{
		enablePeriodicUpdate_global = true;
	}

	return;
}

void menuAlarm( void )
{
	switch( menuButtonRead( 0, 0, 0, 0 ) )
	{
		case 0:
		case 1:
		case 2:
		case 3:
			menuState_global = oldMenuState_global;
			if( alarmActive_global == 1 )
			{
				alarm1Silence_global = true;
			}
			if( alarmActive_global == 2 )
			{
				alarm2Silence_global = true;
			}
			alarmToResume_global = 0;
			alarmActive_global = 0;
			alarmRemainingSets_global = 0;
			break;
	}

	char percentRemBuf[BUF_SIZE_INT];

	itoa( percentRemBuf, percentRem_global, 10 );

	writeToDisplay( "Warning: Low power! Only ", 0, 0 );

	writeToDisplay( percentRemBuf, 25, -2 );
	writeToDisplay( "% remaining.                         Clear Alarm     ", 27, 0 );

	return;
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
	writeToDisplay( softKeys_BackUpDownOK_module, 60, 0 );

	enablePeriodicUpdate_global = false;

	return;
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
			pwLength_module = 0;
	}

	writeToDisplay( "Main Menu     2 of 6* Admin Menu          Modules ", 0, 60 );
	writeToDisplay( softKeys_BackUpDownOK_module, 60, 0 );

	return;
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
			break;
	}

	writeToDisplay( "Main Menu     3 of 6* Modules             Statistics", 0, 60 );
	writeToDisplay( softKeys_BackUpDownOK_module, 60, 0 );

	return;
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
	writeToDisplay( softKeys_BackUpDownOK_module, 60, 0 );

	return;
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
	writeToDisplay( softKeys_BackUpDownOK_module, 60, 0 );

	return;
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
	writeToDisplay( softKeys_BackUpTopOK_module, 60, 0 );

	return;
}

void menuAlarm1( void )
{
	switch( menuButtonRead( MENU_MAIN_1, MENU_ALARM_4, MENU_ALARM_2, 0 ) )
	{
		case 0:
			com_command_setRemoteAlarm( );
			break;
		case 3:
			if( audibleAlarm_global == true )
			{
				audibleAlarm_global = false;
			}
			else
			{
				audibleAlarm_global = true;
			}
	}

	writeToDisplay( "Alarm Options 1 of 3* Audible Alarm  ", 0, 0 );

	if( audibleAlarm_global == true )
	{
		writeToDisplay( "On ", 37, 0 );
	}
	else
	{
		writeToDisplay( "Off", 37, 0 );
	}

	writeToDisplay( "  Alarm 1 Power  ", 40, 0 );

	if( alarm1PercentThreshold_global > 0 )
	{
		char alarmOnePowerBuf[BUF_SIZE_INT];

		itoa( alarmOnePowerBuf, alarm1PercentThreshold_global, 10 );

		writeToDisplay( alarmOnePowerBuf, 57, -2 );
		writeToDisplay( "%", 59, 0 );
	}
	else
	{
		writeToDisplay( "Off", 57, 0 );
	}

	
	writeToDisplay( softKeys_BackUpDownOK_module, 60, 0 );

	return;
}

void menuAlarm2( void )
{
	switch( menuButtonRead( MENU_MAIN_1, MENU_ALARM_1, MENU_ALARM_4, MENU_ALARM_3 ) )
	{
		case 0:
			com_command_setRemoteAlarm( );
			break;
		case 3:
			tempPercent_global = alarm1PercentThreshold_global;
	}

	writeToDisplay( "Alarm Options 2 of 3* Alarm 1 Power  ", 0, 0 );

	if( alarm1PercentThreshold_global > 0 )
	{
		char alarmOnePowerBuf[ BUF_SIZE_LONG];

		itoa( alarmOnePowerBuf, alarm1PercentThreshold_global, 10 );

		writeToDisplay( alarmOnePowerBuf, 37, -2 );
		writeToDisplay( "%", 39, 0 );
	}
	else
	{
		writeToDisplay( "Off", 37, 0 );
	}

	writeToDisplay( "  Alarm 2 Power  ", 40, 0 );

	if( alarm2PercentThreshold_global > 0 )
	{
		char alarmTwoPowerBuf[ BUF_SIZE_INT];

		itoa( alarmTwoPowerBuf, alarm2PercentThreshold_global, 10 );

		writeToDisplay( alarmTwoPowerBuf, 57, -2 );
		writeToDisplay( "%", 59, 0 );
	}
	else
	{
		writeToDisplay( "Off", 57, 0 );
	}

	writeToDisplay( softKeys_BackUpDownOK_module, 60, 0 );

	return;
}

void menuAlarm3( void )
{
	switch( menuButtonRead( MENU_ALARM_2, 0, 0, MENU_ALARM_2 ) )
	{
		case 1:
			if( tempPercent_global >= 20 )
			{
				tempPercent_global += 5;
			}
			else
			{
				tempPercent_global += 1;
			}
			
			if( tempPercent_global > 95)
			{
				tempPercent_global = 95;
			}
			break;

		case 2:
			if( tempPercent_global > 20 )
			{
				tempPercent_global -= 5;
			}
			else
			{
				tempPercent_global -= 1;
			}
			
			if( tempPercent_global < 0 )
			{
				tempPercent_global = 0;
			}
			break;

		case 3:
			alarm1PercentThreshold_global = tempPercent_global;
			alarm1Hit_global = false;
			com_command_setRemoteAlarm( );
	}

	writeToDisplay( "Alarm Options 2 of 3* Alarm 1 Power ", 0, 0 );

	writeToDisplay( rightArrow_module, 36, 0 );

	if( tempPercent_global > 0 )
	{
		char tempPercentBuf[BUF_SIZE_INT];

		itoa( tempPercentBuf, tempPercent_global, 10 );

		writeToDisplay( tempPercentBuf, 37, -2 );
		writeToDisplay( "%", 39, 0 );
	}
	else
	{
		writeToDisplay( "Off", 37, 0 );
	}

	writeToDisplay( "  Alarm 2 Power  ", 40, 0 );

	if( alarm2PercentThreshold_global > true )
	{
		char alarmTwoPowerBuf[ BUF_SIZE_INT];

		itoa( alarmTwoPowerBuf, alarm2PercentThreshold_global, 10 );

		writeToDisplay( alarmTwoPowerBuf, 57, -2 );
		writeToDisplay( "%", 59, 0 );
	}
	else
	{
		writeToDisplay( "Off", 57, 0 );
	}

	writeToDisplay( softKeys_BackUpDownSave_module, 60, 0 );

	return;
}

void menuAlarm4( void )
{

	switch( menuButtonRead( MENU_MAIN_1, MENU_ALARM_2, MENU_ALARM_1, MENU_ALARM_5 ) )
	{
		case 0:
			com_command_setRemoteAlarm( );
			break;
		case 3:
			tempPercent_global = alarm2PercentThreshold_global;
	}

	writeToDisplay( "Alarm Options 3 of 3  Alarm 1 Power  ", 0, 0 );

	if( alarm1PercentThreshold_global > 0 )
	{
		char alarmOnePowerBuf[ BUF_SIZE_INT];

		itoa( alarmOnePowerBuf, alarm1PercentThreshold_global, 10 );

		writeToDisplay( alarmOnePowerBuf, 37, -2 );
		writeToDisplay( "%", 39, 0 );
	}
	else
	{
		writeToDisplay( "Off", 37, 0 );
	}

	writeToDisplay( "* Alarm 2 Power  ", 40, 0 );

	if( alarm2PercentThreshold_global > 0 )
	{
		char alarmTwoPowerBuf[ BUF_SIZE_INT];

		itoa( alarmTwoPowerBuf, alarm2PercentThreshold_global, 10 );

		writeToDisplay( alarmTwoPowerBuf, 57, -2 );
		writeToDisplay( "%", 59, 0 );
	}
	else
	{
		writeToDisplay( "Off", 57, 0 );
	}

	writeToDisplay( softKeys_BackUpTopOK_module, 60, 0 );

	return;
}

void menuAlarm5( void )
{

	switch( menuButtonRead( MENU_ALARM_4, 0, 0, MENU_ALARM_4 ) )
	{
		case 1:
			if( tempPercent_global >= 20 )
			{
				tempPercent_global += 5;
			}
			else
			{
				tempPercent_global += 1;
			}
			
			if( tempPercent_global > 95)
			{
				tempPercent_global = 95;
			}
			break;

		case 2:
			if( tempPercent_global > 20 )
			{
				tempPercent_global -= 5;
			}
			else
			{
				tempPercent_global -= 1;
			}
			
			if( tempPercent_global < 0 )
			{
				tempPercent_global = 0;
			}
			break;

		case 3:
			alarm2PercentThreshold_global = tempPercent_global;
			alarm2Hit_global = false;
			com_command_setRemoteAlarm( );
	}

	writeToDisplay( "Alarm Options 3 of 3  Alarm 1 Power  ", 0, 0 );

	if( alarm1PercentThreshold_global > 0 )
	{
		char alarmOnePowerBuf[BUF_SIZE_INT];

		itoa( alarmOnePowerBuf, alarm1PercentThreshold_global, 10 );

		writeToDisplay( alarmOnePowerBuf, 37, -2 );
		writeToDisplay( "%", 39, 0 );
	}
	else
	{
		writeToDisplay( "Off", 37, 0 );
	}

	writeToDisplay( "* Alarm 2 Power ", 40, 0 );
	writeToDisplay( rightArrow_module, 56, 0 );

	if( tempPercent_global > 0 )
	{
		char tempPercentBuf[ BUF_SIZE_INT];

		itoa( tempPercentBuf, tempPercent_global, 10 );

		writeToDisplay( tempPercentBuf, 57, -2 );
		writeToDisplay( "%", 59, 0 );
	}
	else
	{
		writeToDisplay( "Off", 57, 0 );
	}

	writeToDisplay( softKeys_BackUpDownSave_module, 60, 0 );

	return;
}

void menuStatistics( void )
{
	char previousDayUsedBuf[BUF_SIZE_LONG];
	char totalUsedBuf[BUF_SIZE_LONG];

	menuButtonRead( MENU_MAIN_4, MENU_MAIN_4, MENU_MAIN_4, MENU_MAIN_4 );

	ltoa( previousDayUsedBuf, previousDayUsed_global, 10 );
	ltoa( totalUsedBuf, totalUsed_global, 10 );

	writeToDisplay( "Yesterday's usage:  ", 0, 0 );
	writeToDisplay( previousDayUsedBuf, 20, -17 );
	writeToDisplay( " Wh", 37, 0 );
	writeToDisplay( "Total usage:", 40, 20 );
	writeToDisplay( totalUsedBuf, 60, -17 );
	writeToDisplay( " Wh", 77, 0 );

	return;
}

void menuPowerFailTimes( void )
{
	menuButtonRead( MENU_MAIN_5, MENU_MAIN_5, MENU_MAIN_5, MENU_MAIN_5 );

	writeToDisplay( "Last Power Failure  ", 0, 0 );
	writeToDisplay( "Lost     ", 20, 0 );
	writeToDisplay( powerDownTime_global, 29, 0 );
	writeToDisplay( "Restored ", 40, 0 );
	writeToDisplay( powerUpTime_global, 49, 0 );
	writeToDisplay( "Back", 60, 20 );

	return;
}

void menuAbout( void )
{
	menuButtonRead( MENU_MAIN_6, MENU_MAIN_6, MENU_MAIN_6, MENU_MAIN_6 );

	writeToDisplay( "Power Code: ", 0, 0 );
	writeToDisplay( powerBoxCodeVersionString_global, 12, 8 );
	writeToDisplay( "Disp Code:  ", 20, 0 );
	writeToDisplay( DISPLAY_BOX_CODE_VERSION, 32, 8 );
	writeToDisplay( " Messiah University     Collaboratory    ", 40, 0 );

	return;
}

void menuAdminLogin( void )
{
	if( pwLength_module != 6 )
	{
		switch( menuButtonRead( 0, 0, 0, 0 ) )
		{
			case 0:
				passwordInput_module[pwLength_module++] = '1';
				break;
			case 1:
				passwordInput_module[pwLength_module++] = '2';
				break;
			case 2:
				passwordInput_module[pwLength_module++] = '3';
				break;
			case 3:
				passwordInput_module[pwLength_module++] = '4';
		}
	}
	else if( passwordInput_module[0] == passwordSet_global[0] &&
			 passwordInput_module[1] == passwordSet_global[1] &&
			 passwordInput_module[2] == passwordSet_global[2] &&
			 passwordInput_module[3] == passwordSet_global[3] &&
			 passwordInput_module[4] == passwordSet_global[4] &&
			 passwordInput_module[5] == passwordSet_global[5] )
	{
		menuState_global = MENU_ADMIN_1;
		com_command_readRemoteTime( );
		//TODO unnest the return statements
		return;
	}
	else
	{
		menuState_global = MENU_HOME_BASIC;
		//TODO unnest the return statements
		return;
	}

	writeToDisplay( "Administrator Login", 0, 44 );

	switch( pwLength_module )
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

	return;
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
			timeSetPos_global = 1;

			// set temporary time variables to what's in the RTCC
			rtccReadTime( );
			tempHour_global = timeHour_global;
			tempMin_global = timeMinute_global;
			tempMonth_global = timeMonth_global;
			tempDay_global = timeDay_global;
			tempYear_global = timeYear_global;
	}

	writeToDisplay( "Admin Menu    1 of 6* Set Time/Date       Set Power Alloc", 0, 60 );
	writeToDisplay( softKeys_BackUpDownOK_module, 60, 0 );

	return;
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
			tempAlloc_module = energyAllocated_global;
	}

	writeToDisplay( "Admin Menu    2 of 6* Set Power Alloc     Emergency Options", 0, 60 );
	writeToDisplay( softKeys_BackUpDownOK_module, 60, 0 );

	return;
}

void menuAdmin3( void )
{
	switch( menuButtonRead( MENU_MAIN_2, MENU_ADMIN_2, MENU_ADMIN_4, MENU_EMERGENCY_1 ) )
	{
		case 1:
			com_command_readRemoteEnergyAllocation( );
	}

	writeToDisplay( "Admin Menu    3 of 6* Emergency Options   Change Password", 0, 60 );
	writeToDisplay( softKeys_BackUpDownOK_module, 60, 0 );

	return;
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
			pwLength_module = 0;
			newPassword_module[0] = newPassword_module[2] = newPassword_module[4] = newPassword_module[6] =
					newPassword_module[8] = newPassword_module[10] = '_';
			newPassword_module[1] = newPassword_module[3] = newPassword_module[5] = newPassword_module[7] =
					newPassword_module[9] = ' ';
			newPassword_module[11] = 0;
	}

	writeToDisplay( "Admin Menu    4 of 6* Change Password     Change Reset Time", 0, 60 );
	writeToDisplay( softKeys_BackUpDownOK_module, 60, 0 );

	return;
}

void menuAdmin5( void )
{
	switch( menuButtonRead( MENU_MAIN_2, MENU_ADMIN_4, MENU_ADMIN_6, MENU_SET_RESET_TIME ) )
	{
		case 2:
			com_command_readRemoteRelay( );
			break;
		case 3:
			tempResetHour_global = resetTimeHour_global;
			tempResetMinute_global = resetTimeMinute_global;
	}

	writeToDisplay( "Admin Menu    5 of 6* Change Reset Time   Relay Control", 0, 60 );
	writeToDisplay( softKeys_BackUpDownOK_module, 60, 0 );

	return;
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
			relayModeTemp_module = relayMode_global;
	}

	writeToDisplay( "Admin Menu    6 of 6  Change Reset Time * Relay Control", 0, 60 );
	writeToDisplay( softKeys_BackUpTopOK_module, 60, 0 );

	return;
}

void menuSetTime( void )
{

	switch( menuButtonRead( 0, 0, 0, 0 ) )
	{
		case 0:
			timeSetPos_global--;

			if( !timeSetPos_global )
			{
				if( isBooting_global )
				{
					timeSetPos_global = 1;
				}
				else
				{
					menuState_global = MENU_ADMIN_1;
				}
			}
			break;
		case 1:
			switch( timeSetPos_global )
			{
				case 1:
					tempHour_global = ( tempHour_global + 1 ) % 24;
					break;

				case 2:
					tempMin_global = ( tempMin_global + 1 ) % 60;
					break;

				case 3:
					tempDay_global--;
					tempDay_global = ( tempDay_global + 1 ) % 31;
					tempDay_global++;
					break;

				case 4:
					tempMonth_global--;
					tempMonth_global = ( tempMonth_global + 1 ) % 12;
					tempMonth_global++;
					break;

				case 5:
					tempYear_global++;
					if( tempYear_global > 99 )
						tempYear_global = 14;
			}
			break;
		case 2:
			switch( timeSetPos_global )
			{
				case 1:
					tempHour_global--;
					if( tempHour_global < 0 )
					{
						tempHour_global = 23;
					}
					break;

				case 2:
					tempMin_global--;
					if( tempMin_global < 0 )
					{
						tempMin_global = 59;
					}
					break;

				case 3:
					tempDay_global--;
					if( tempDay_global < 1 )
					{
						tempDay_global = 31;
					}
					break;

				case 4:
					tempMonth_global--;
					if( tempMonth_global < 1 )
					{
						tempMonth_global = 12;
					}
					break;

				case 5:
					tempYear_global--;
					if( tempYear_global < 14 )
					{
						tempYear_global = 99;
					}
			}
			break;

		case 3:
			timeSetPos_global++;
			if( timeSetPos_global == 6 )
			{
				if( writeTime( tempYear_global, tempMonth_global, tempDay_global, tempHour_global, tempMin_global, 0 ) == 1 )
				{
					com_command_setRemoteTime( );
					//                    lastUpdateSecond = timeSecond; // correct periodic update timing
					if( isBooting_global )
					{
						menuState_global = MENU_POWER_RESET;
						//isBooting = 0;
						com_command_readRemoteResetTime( );
					}
					else
					{
						menuState_global = MENU_ADMIN_1;
					}
				}
				else
				{
					menuState_global = MENU_BAD_DATE;
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
	writeToDisplay( tempClockStr_global, 25, 18 );
	writeToDisplay( tempCalStr_global, 43, 17 );

	if( timeSetPos_global != 5 )
	{
		writeToDisplay( "Back  Up  Down  Next", 60, 0 );
	}
	else
	{
		writeToDisplay( softKeys_BackUpDownSave_module, 60, 0 );
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

	return;
}

void menuSetPower( void )
{
	switch( menuButtonRead( MENU_ADMIN_2, 0, 0, 0 ) )
	{
		case 1:
			if( tempAlloc_module < 5 )
				tempAlloc_module++;
			else if( tempAlloc_module < 50 )
				tempAlloc_module += 5;
			else if( tempAlloc_module < 5000 )
				tempAlloc_module += 50;
			else if( tempAlloc_module < 10000 )
				tempAlloc_module += 100;
			else if( tempAlloc_module < 31999 )
				tempAlloc_module += 250;
			break;

		case 2:
			if( tempAlloc_module > 10000 )
				tempAlloc_module -= 250;
			else if( tempAlloc_module > 5000 )
				tempAlloc_module -= 100;
			else if( tempAlloc_module > 50 )
				tempAlloc_module -= 50;
			else if( tempAlloc_module > 5 )
				tempAlloc_module -= 5;
			else if( tempAlloc_module > 0 )
				tempAlloc_module--;

			// to fix unexpected boundary conditions
			if( tempAlloc_module < 0 || tempAlloc_module > 32000 )
			{
				tempAlloc_module = 0;
			}
			break;

		case 3:

			// check if the power will be turned off by this action
			// if so, prompt the user for confirmation
			if( energyUsed_global < tempAlloc_module )
			{
				menuState_global = MENU_ADMIN_2;

				energyAllocated_global = tempAlloc_module;
				com_command_setRemoteEnergyAllocation( );
			}
			else
			{
				menuState_global = MENU_SHUT_OFF_WARNING;
				oldPowerMenuState_module = MENU_SET_POWER;
			}

	}

	char tempAllocBuf[BUF_SIZE_INT];

	itoa( tempAllocBuf, tempAlloc_module, 10 );

	writeToDisplay( "Set Power Allocation  ", 0, 0 );
	writeToDisplay( tempAllocBuf, 22, -5 );
	writeToDisplay( " Wh per day", 27, 33 );
	writeToDisplay( softKeys_BackUpDownSave_module, 60, 0 );

	return;
}

void menuSetResetTime( void )
{
	switch( menuButtonRead( MENU_ADMIN_5, 0, 0, MENU_ADMIN_5 ) )
	{
		case 1:
			if( !tempResetMinute_global )
			{
				tempResetMinute_global = 30;
			}
			else
			{
				tempResetMinute_global = 0;
				tempResetHour_global = ( tempResetHour_global + 1 ) % 24;
			}
			break;

		case 2:
			if( tempResetMinute_global == 30 )
			{
				tempResetMinute_global = 0;
			}
			else
			{
				tempResetMinute_global = 30;
				tempResetHour_global--;

				if( tempResetHour_global > 23 )
				{
					tempResetHour_global = 23;
				}
			}
			break;

		case 3:
			resetTimeHour_global = tempResetHour_global;
			resetTimeMinute_global = tempResetMinute_global;

			com_command_setRemoteResetTime( );
	}

	tempResetTimeString_module[0] = ( tempResetHour_global / 10 ) + 0x30;
	tempResetTimeString_module[1] = ( tempResetHour_global % 10 ) + 0x30;
	tempResetTimeString_module[2] = ':';
	char rtm[20];

	itoa( rtm, tempResetMinute_global, 10 );

	if( rtm[1] == CHAR_NULL )
	{
		tempResetTimeString_module[3] = '0';
		tempResetTimeString_module[4] = rtm[0];
	}
	else
	{
		tempResetTimeString_module[3] = rtm[0];
		tempResetTimeString_module[4] = rtm[1];
	}

	tempResetTimeString_module[5] = CHAR_NULL;


	//    if( !tempResetMinute )
	//	tempResetTimeString[3] = '0';
	//    else
	//	tempResetTimeString[3] = '3';
	//    tempResetTimeString[4] = '0';
	//    tempResetTimeString[5] = 0;

	writeToDisplay( "Set Reset Time", 0, 27 );
	writeToDisplay( tempResetTimeString_module, 27, 33 );
	writeToDisplay( softKeys_BackUpDownSave_module, 60, 0 );

	return;
}

void menuEmergency1( void )
{
	switch( menuButtonRead( MENU_ADMIN_3, MENU_EMERGENCY_2, MENU_EMERGENCY_2, MENU_EMERGENCY_3 ) )
	{
		case 3:
			emerButtonAllocTemp_module = emerButtonEnergyAllocate_global;
	}

	writeToDisplay( "Emergency Options   * Emer Button    ", 0, 0 );

	if( emerButtonEnable_global > 0 )
	{
		char emerButtonAllocBuf[BUF_SIZE_INT];

		itoa( emerButtonAllocBuf, emerButtonEnergyAllocate_global, 10 );

		writeToDisplay( emerButtonAllocBuf, 37, -3 );
		emerButtonEnable_global = 1;
	}
	else
	{
		writeToDisplay( "Off", 37, 0 );
		emerButtonEnable_global = 0;
	}

	char emerAllocNowBuf[ BUF_SIZE_INT];

	itoa( emerAllocNowBuf, emerAllocNow_global, 10 );

	writeToDisplay( "  Emer Alloc Now  ", 40, 0 );

	writeToDisplay( emerAllocNowBuf, 57, -3 );

	writeToDisplay( softKeys_BackUpDownOK_module, 60, 0 );

	return;
}

void menuEmergency2( void )
{
	menuButtonRead( MENU_ADMIN_3, MENU_EMERGENCY_1, MENU_EMERGENCY_1, MENU_EMERGENCY_4 );

	writeToDisplay( "Emergency Options     Emer Button    ", 0, 0 );

	if( emerButtonEnable_global == 1 )
	{
		char emerButtonAllocBuf[ BUF_SIZE_INT];

		itoa( emerButtonAllocBuf, emerButtonEnergyAllocate_global, 10 );

		writeToDisplay( emerButtonAllocBuf, 37, -3 );
		emerButtonEnable_global = 1;
	}
	else
	{
		writeToDisplay( "Off", 37, 0 );
		emerButtonEnable_global = 0;
	}

	char emerAllocNowBuf[ BUF_SIZE_INT];

	itoa( emerAllocNowBuf, emerAllocNow_global, 10 );

	writeToDisplay( "* Emer Alloc Now ", 40, 0 );
	writeToDisplay( emerAllocNowBuf, 57, -3 );

	writeToDisplay( softKeys_BackUpDownOK_module, 60, 0 );

	return;
}

void menuEmergency3( void )
{

	switch( menuButtonRead( MENU_EMERGENCY_1, 0, 0, MENU_EMERGENCY_1 ) )
	{
		case 1:
			if( !emerButtonEnable_global )
			{
				emerButtonEnable_global = 1;
				emerButtonAllocTemp_module = 50;
			}
			else
			{
				if( emerButtonAllocTemp_module < 950 )
				{
					emerButtonAllocTemp_module += 50;
				}
			}
			break;

		case 2:
			if( emerButtonEnable_global )
			{
				if( emerButtonAllocTemp_module < 100 )
				{
					emerButtonEnable_global = 0;
					emerButtonAllocTemp_module = 50;
				}
				else
				{
					emerButtonAllocTemp_module -= 50;
				}
			}
			break;

		case 3:
			emerButtonEnergyAllocate_global = emerButtonAllocTemp_module;
			emerAllocSend_global = 0;
			com_command_setRemoteEmergency( );
	}

	writeToDisplay( "Emergency Options   * Emer Button   ", 0, 0 );
	writeToDisplay( rightArrow_module, 36, 0 );

	if( emerButtonEnable_global > 0 )
	{
		char emerButtonAllocTempBuf[ BUF_SIZE_INT];

		itoa( emerButtonAllocTempBuf, emerButtonAllocTemp_module, 10 );

		writeToDisplay( emerButtonAllocTempBuf, 37, -3 );
		emerButtonEnable_global = 1;
	}
	else
	{
		writeToDisplay( "Off", 37, 0 );
		emerButtonEnable_global = 0;
	}

	char emerAllocNowBuf[ BUF_SIZE_INT];

	itoa( emerAllocNowBuf, emerAllocNow_global, 10 );

	writeToDisplay( "  Emer Alloc Now ", 40, 0 );

	writeToDisplay( emerAllocNowBuf, 57, -3 );

	writeToDisplay( softKeys_BackUpDownSave_module, 60, 0 );

	return;
}

void menuEmergency4( void )
{

	switch( menuButtonRead( MENU_EMERGENCY_2, 0, 0, MENU_EMERGENCY_2 ) )
	{
		case 1:
			if( emerAllocNow_global < 950 )
			{
				emerAllocNow_global += 50;
			}
			break;
		case 2:
			if( emerAllocNow_global >= 100 )
			{
				emerAllocNow_global -= 50;
			}
			break;
		case 3:
			emerAllocSend_global = emerAllocNow_global;
			com_command_setRemoteAllocationAdd( );
			emerAllocSend_global = 0;
	}

	writeToDisplay( "Emergency Options     Emer Button    ", 0, 0 );

	if( emerButtonEnable_global == 1 )
	{
		char emerButtonAllocBuf[ BUF_SIZE_INT];

		itoa( emerButtonAllocBuf, emerButtonEnergyAllocate_global, 10 );

		writeToDisplay( emerButtonAllocBuf, 37, -3 );
	}
	else
	{
		writeToDisplay( "Off", 37, 0 );
		emerButtonEnable_global = 0;
	}

	char emerAllocNowBuf[ BUF_SIZE_INT];

	itoa( emerAllocNowBuf, emerAllocNow_global, 10 );

	writeToDisplay( "* Emer Alloc Now", 40, 0 );
	writeToDisplay( rightArrow_module, 56, 0 );
	writeToDisplay( emerAllocNowBuf, 57, -3 );

	writeToDisplay( softKeys_BackUpDownSave_module, 60, 0 );

	return;
}

void menuModules( void )
{

	static int moduleIndex = 0;

	// this should be replaced with the modules info
	switch( menuButtonRead( MENU_MAIN_3, 0, 0, 0 ) )
	{
		case 1:
			moduleIndex--;
			if( moduleIndex < 0 )
			{
				moduleIndex = ( MODULE_COUNT - 1 );
			}
			break;
		case 2:
			moduleIndex++;
			if( moduleIndex >= MODULE_COUNT )
			{
				moduleIndex = 0;
			}
			break;
		case 3:
			menuState_global = MENU_MAIN_3;
	}

	char moduleIndexBuf[BUF_SIZE_INT];

	itoa( moduleIndexBuf, moduleIndex, 10 );

	writeToDisplay( moduleInfo_global[moduleIndex].info0, 0, 10 );
	writeToDisplay( moduleInfo_global[moduleIndex].info1, 10, 10 );
	writeToDisplay( moduleInfo_global[moduleIndex].info2, 20, 10 );
	writeToDisplay( moduleInfo_global[moduleIndex].info3, 30, 10 );
	writeToDisplay( moduleInfo_global[moduleIndex].info4, 40, 20 );

	writeToDisplay( moduleIndexBuf, 59, 1 );
	writeToDisplay( "-", 58, 1 );

	if( moduleIndex == ( MODULE_COUNT - 1 ) )
	{
		writeToDisplay( softKeys_BackUpTopOK_module, 60, 0 );
	}
	else
	{
		writeToDisplay( softKeys_BackUpDownOK_module, 60, 0 );
	}

	return;
}

void menuSetRelay( void )
{
	// need to figure out how the relay menu system works
	// then modify it for the 3 options
	//      off, on, auto
	// steps
	//  run the menu as is
	//      see how it is laid out
	//      see how the variables below might affect its functionality
	//      figure out where the variables are used (if more than just here)
	//      add auto
	//      make the value 0, 1, 2 work
	//          0   =   off
	//          1   =   on
	//          2   =   auto

	switch( menuButtonRead( MENU_ADMIN_6, 0, 0, MENU_ADMIN_6 ) )
	{
		case 0:
			// do nothing - menu automatically changed
			break;
		case 1:
		case 2:
			// switch modes
			relayModeTemp_module++;
			if( relayModeTemp_module > 2 )
			{
				relayModeTemp_module = 0;
			}
			break;
		case 3:
			relayMode_global = relayModeTemp_module;
			com_command_setRemoteRelay( );
			break;
	}

	writeToDisplay( "   Relay Function   ", 0, 20 );
	writeToDisplay( "    ", 20, 4 );
	writeToDisplay( "Off", 25, 3 );
	writeToDisplay( "    ", 40, 4 );
	writeToDisplay( "On", 45, 15 );
	writeToDisplay( "    ", 28, 4 );
	writeToDisplay( "Auto", 32, 8 );

	switch( relayModeTemp_module )
	{
		case 0: //  off
			writeToDisplay( rightArrow_module, 24, 1 );
			writeToDisplay( " ", 44, 1 );
			writeToDisplay( " ", 31, 1 );
			break;
		case 1: //  on
			writeToDisplay( " ", 24, 1 );
			writeToDisplay( rightArrow_module, 44, 1 );
			writeToDisplay( " ", 31, 1 );
			break;
		case 2: //  auto
			writeToDisplay( " ", 24, 1 );
			writeToDisplay( " ", 44, 1 );
			writeToDisplay( rightArrow_module, 31, 1 );
			break;
	}

	writeToDisplay( "Back  *Switch*  Save", 60, 0 );

	return;
}

void menuBadDate( void )
{
	if( menuButtonRead( MENU_SET_TIME, MENU_SET_TIME, MENU_SET_TIME, MENU_SET_TIME ) != -1 )
	{
		timeSetPos_global--;
	}

	writeToDisplay( "Invalid date entered", 0, 60 );
	writeToDisplay( "Return", 60, 20 );

	return;
}

void menuShutOffWarning( void )
{

	switch( menuButtonRead( oldPowerMenuState_module, oldPowerMenuState_module, 0, 0 ) )
	{
		case 2:
		case 3:
			if( oldPowerMenuState_module == MENU_SET_POWER )
			{
				menuState_global = MENU_ADMIN_2;
			}
			else
			{
				menuState_global = MENU_MAIN_3;
			}

			energyAllocated_global = tempAlloc_module;
			com_command_setRemoteEnergyAllocation( );
	}

	writeToDisplay( "Continuing will     cause power to shut off immediately!    Return      Continue", 0, 0 );

	return;
}

void menuPasswordChange( void )
{
	if( pwLength_module < 6 )
	{
		switch( menuButtonRead( 0, 0, 0, 0 ) )
		{
			case 0:
				newPassword_module[pwLength_module * 2] = '1';
				passwordInput_module[pwLength_module++] = '1';
				break;
			case 1:
				newPassword_module[pwLength_module * 2] = '2';
				passwordInput_module[pwLength_module++] = '2';
				break;
			case 2:
				newPassword_module[pwLength_module * 2] = '3';
				passwordInput_module[pwLength_module++] = '3';
				break;
			case 3:
				newPassword_module[pwLength_module * 2] = '4';
				passwordInput_module[pwLength_module++] = '4';
		}

	}
	else
	{
		switch( menuButtonRead( 0, 0, 0, MENU_ADMIN_4 ) )
		{
			case 0:
				pwLength_module = 0;
				//FIX DONE
				//stringCopy( "_ _ _ _ _ _", newPassword );
				newPassword_module[0] = '_';
				newPassword_module[1] = ' ';
				newPassword_module[2] = '_';
				newPassword_module[3] = ' ';
				newPassword_module[4] = '_';
				newPassword_module[5] = ' ';
				newPassword_module[6] = '_';
				newPassword_module[7] = ' ';
				newPassword_module[8] = '_';
				newPassword_module[9] = ' ';
				newPassword_module[10] = '_';
				newPassword_module[11] = CHAR_NULL;



				break;
			case 1:
			case 2:
				break;
			case 3:
				passwordSet_global[0] = passwordInput_module[0];
				passwordSet_global[1] = passwordInput_module[1];
				passwordSet_global[2] = passwordInput_module[2];
				passwordSet_global[3] = passwordInput_module[3];
				passwordSet_global[4] = passwordInput_module[4];
				passwordSet_global[5] = passwordInput_module[5];
				com_command_setRemotePassword( );
				pwLength_module = 0;
		}

	}

	writeToDisplay( "Enter new password:", 0, 44 );

	writeToDisplay( newPassword_module, 44, 16 );

	if( pwLength_module < 6 )
	{
		writeToDisplay( "1     2      3     4", 60, 20 );
	}
	else
	{
		writeToDisplay( "Clear           Save", 60, 0 );
	}

	return;
}

void menuPowerReset( void )
{

	switch( menuButtonRead( MENU_HOME_BASIC, 0, 0, MENU_HOME_BASIC ) )
	{
		case 3:
			com_command_doReset( ); // fall through on purpose
		case 0:
			isBooting_global = 0;
	}

	writeToDisplay( "Is it after the     reset time?", 0, 46 );

	char resetHourBuf[BUF_SIZE_INT];

	itoa( resetHourBuf, resetTimeHour_global, 10 );


	if( resetTimeHour_global < 10 )
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
	if( resetTimeMinute_global )
	{
		writeToDisplay( "3", 50, 0 );
	}
	else
	{
		writeToDisplay( "0", 50, 0 );
	}
	writeToDisplay( "0)       No               Yes", 51, 0 );

	return;
}

void writeBarGraph( void )
{
	barGraph_module[20] = 0;
	unsigned char counter;
	unsigned char filled;

	filled = percentRem_global / 5;

	for( counter = 0; counter < 20; counter++ )
	{
		if( counter < filled )
		{
			barGraph_module[counter] = 0xFF;
		}
		else
		{
			barGraph_module[counter] = '_';
		}
	}

	if( percentRem_global < 5 && energyUsed_global < energyAllocated_global )
	{
		barGraph_module[0] = '|';
	}

	return;
}

void calcPercentRem( void )
{
	percentRem_global = ( energyAllocated_global - energyUsed_global ) * 100 / energyAllocated_global;

	if( ( percentRem_global < 0 ) || ( energyUsed_global > energyAllocated_global ) )
	{
		percentRem_global = 0;
	}
	else if( percentRem_global > 100 )
	{
		percentRem_global = 100;
	}

	return;
}

void calcTimeRemaining( void )
{
	// variables:
	// timeRemHour, timeRemMinute, timeRemSecond for actual time
	// timeRemUpdate for running average
	// timeRem1, timeRem2, timeRem3, timeRem4, timeRem5 to store values

	if( timeSecond_global != timeRemUpdate_module )
	{
		timeRemainingString_module[2] = timeRemainingString_module[5] = ':';
		timeRemainingString_module[8] = 0;

		timeRemUpdate_module = timeSecond_global;
		timeRem1_module = timeRem2_module;
		timeRem2_module = timeRem3_module;
		timeRem3_module = timeRem4_module;
		timeRem4_module = timeRem5_module;
		timeRem5_module = powerLoad_global;

		unsigned long timeRemAvg;

		timeRemAvg = ( timeRem1_module + timeRem2_module + timeRem3_module + timeRem4_module + timeRem5_module ) / 5;

		if( !timeRemAvg )
		{
			timeRemainingString_module[0] = timeRemainingString_module[3] = timeRemainingString_module[6] = '5';
			timeRemainingString_module[1] = timeRemainingString_module[4] = timeRemainingString_module[7] = '9';
		}
		else
		{
			long powerRemaining = energyAllocated_global - energyUsed_global;
			unsigned long timeRemaining = ( 3600 * powerRemaining ) / timeRemAvg;
			timeRemHour_module = timeRemaining / 3600;
			timeRemMinute_module = ( timeRemaining / 60 ) - ( 60 * timeRemHour_module );
			timeRemSecond_module = timeRemaining % 60;

			if( timeRemHour_module > 59 )
			{
				timeRemHour_module = 59;
			}

			timeRemainingString_module[0] = timeRemHour_module / 10 + 0x30;
			timeRemainingString_module[1] = timeRemHour_module % 10 + 0x30;
			timeRemainingString_module[3] = timeRemMinute_module / 10 + 0x30;
			timeRemainingString_module[4] = timeRemMinute_module % 10 + 0x30;
			timeRemainingString_module[6] = timeRemSecond_module / 10 + 0x30;
			timeRemainingString_module[7] = timeRemSecond_module % 10 + 0x30;
		}
	}

	return;
}

void dispUpdate( void )
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

			if( currentDisplay_module[lineOffset + j] !=
			 nextDisplay_module[lineOffset + j] )
			{

				currentDisplay_module[lineOffset + j] =
						nextDisplay_module[lineOffset + j];
				matches = 0;
			}
		}

		if( ( !matches && currentDisplay_module[lineOffset] != 0 ) )
		{

			char tempArray[NUM_LCD_WIDTH + 1];
			tempArray[NUM_LCD_WIDTH] = 0;
			for( k = 0; k < NUM_LCD_WIDTH; k++ )
			{
				tempArray[k] = currentDisplay_module[lineOffset + k];
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

	return;
}
