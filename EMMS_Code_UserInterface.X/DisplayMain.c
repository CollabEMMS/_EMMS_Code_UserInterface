/****************
 INCLUDES
 only include the header files that are required
 ****************/
#include <stdlib.h>
#include "common.h"

#include "Communications.h"
#include "DisplayRTCC.h"
#include "DisplayMenu.h"
#include "DisplayMenuMacros.h"
#include "Delays.h"

/****************
 MACROS
 ****************/
#define BTN_0       _RA2    // Pin 7:  RA2
#define BTN_1       _RA3    // Pin 8:  RA3
#define BTN_2       _RB4    // Pin 9:  RB4
#define BTN_3       _RA4    // Pin 10: RA4

/****************
 VARIABLES
 these are the globals required by only this c file
 there should be as few of these as possible to help keep things clean
 variables required by other c functions should be here and also in the header .h file
 as external
 ****************/
// external
// internal only


unsigned char commError;
unsigned char commErrorTime;
char alarmEnd;
char alarmPulse;

char numBeeps;
char numSets;


/****************
 FUNCTION PROTOTYPES
 only include functions called from within this code
 external functions should be in the header
 ideally these are in the same order as in the code listing
 any functions used internally and externally (prototype here and in the .h file)
	 should be marked
 *****************/
char menuButtonRead( char menu1, char menu2, char menu3, char menu4 );
void init( void );

void initTimer( void );
void initPorts( void );
void enablePullDownResistors( void );


void enableInterrupts( void );

void initVars( void );

void periodicUpdate( void );
//void initDisplayBox( void );
void enableAlarm( void );

//void checkCommFailure( void );
void startAlarm( void );



void initRTCCDisplay( void );
void debugBacklight( bool state );
void debugBacklightToggle( );


/****************
 CODE
 ****************/
/* main
 * Initializes and runs through the main code that is repetitively called
 */
int main( void )
{
	resetTimeSecond = 59;
	resetTimeMinute = 9;
	resetTimeHour = 8;

	init( );

	communications( true );
	static unsigned char lowPriorityCounter = 0;

	//    initDisplayBox( );

	enablePeriodicUpdate = 1;

	while( true )
	{

		// Only do these tasks every 100 - 150 ms
		if( !lowPriorityCounter++ )
		{

			readTime( );
			periodicUpdate( );
			calcPercentRem( );
			calcTimeRemaining( );
			enableAlarm( );
			//	    checkCommFailure( );
			//	    resetWDT( );
			//	    asm( "CLRWDT" ); // reset the watchdog timer in case it is running

		}

		updateMenu( );
		communications( false );
	}
}

/* Functions ******************************************************************/
/* init
 * Calls each individual initialization method
 */
void init( void )
{
	initTimer( );
	initPorts( );
	enablePullDownResistors( );
	initDisplay( );
	initRTCCDisplay( );
	//    initUART( );    now done in the communications code

	enableInterrupts( );
	//    initDisplayBox( );

	readTime( );
	tempHour = timeHour;
	tempMin = timeMinute;
	tempMonth = timeMonth;
	tempDay = timeDay;
	tempYear = timeYear;

	initVars( );
}
/* initVars
 * Initializes variables to their starting values (usually 0)
 */
void initVars( void )
{

	if( menuState != MENU_DEBUG )
		menuState = MENU_HOME_BASIC;

	//FIX DONE
	//    stringCopy( "Unknown ", powerBoxCodeVersionString );

	powerBoxCodeVersionString[0] = 'U';
	powerBoxCodeVersionString[1] = 'n';
	powerBoxCodeVersionString[2] = 'k';
	powerBoxCodeVersionString[3] = 'n';
	powerBoxCodeVersionString[4] = 'o';
	powerBoxCodeVersionString[5] = 'w';
	powerBoxCodeVersionString[6] = 'n';
	powerBoxCodeVersionString[7] = ' ';
	powerBoxCodeVersionString[8] = CHAR_NULL;


	audibleAlarm = 0;
	alarm1Enabled = 0;
	alarm2Enabled = 0;
	alarm1Energy = 0;
	alarm2Energy = 0;
	activeAlarm = 0;
	numBeeps = 4;
	numSets = 4;
	tempPercent = 0;
	emerButtonEnable = 1;
	emerButtonEnergyAllocate = 250;
	emerAllocNow = 50;
	emerAllocSend = 0;

	resetTimeHour = 0;
	resetTimeMinute = 0;
	tempResetHour = 0;
	tempResetMinute = 0;

	relayActive = 1;
	tempRelayActive = 1;
}
/* initPorts
 * initializes ports for I/O
 * disables Int0 interrupt
 */
void initPorts( void )
{

	//OSCCON = 0b0000000010100101;
	ANSA = 0x0000;
	ANSB = 0x0000;
	//PORTA = 0b1111111111111111;
	PORTA = 0x0000;
	PORTB = 0x0000;
	TRISA = 0x0000; // 1 for input, 0 for output
	TRISB = 0x0000;

	TRISBbits.TRISB1 = 1; // UART2
	// Buttons
	TRISAbits.TRISA4 = 1;
	TRISBbits.TRISB4 = 1;
	TRISAbits.TRISA3 = 1;
	TRISAbits.TRISA2 = 1;

	// disable int0 interrupt, just in case it initializes enabled
	_INT0IE = 0;
}
void enablePullDownResistors( void )
{
	// uncomment pins you want to be pull down resistors

	// pin 1 is !MCLR
	//_CN2PDE  = 1;// pin 2
	//_CN3PDE  = 1;// pin 3
	//_CN4PDE  = 1;// pin 4
	//_CN5PDE  = 1;// pin 5
	//_CN6PDE  = 1;// pin 6
	_CN30PDE = 1; // pin 7
	_CN29PDE = 1; // pin 8
	_CN1PDE = 1; // pin 9
	_CN0PDE = 1; // pin 10

	//_CN23PDE = 1;// pin 11
	//_CN22PDE = 1;// pin 12
	//_CN21PDE = 1;// pin 13
	// pin 14 is Vcap
	//_CN14PDE = 1;// pin 15
	//_CN13PDE = 1;// pin 16
	//_CN12PDE = 1;// pin 17
	//_CN11PDE = 1;// pin 18
	// pin 19 is Vss
	// pin 20 is Vdd
}
/* enableInterrupts
 * as named
 */
void enableInterrupts( void )
{
	INTCON1 |= 0b1000000000000000;
}
/* disableInterrupts
 * also as named
 */
void disableInterrupts( void )
{
	INTCON1 &= 0b0111111111111111;
}
void enableAlarm( void )
{
	static char startNextAlarm;

	if( isBooting ) return;

	if( percentRem > 95 )
	{
		alarmOneHit = alarmTwoHit = 0;
	}

	if( alarm1Energy && (percentRem <= alarm1Energy) && !alarmOneHit && !silenceAlarmOne && !activeAlarm && !alarmToResume )
	{

		remainingSets = numSets;
		activeAlarm = 1;
		alarmOneHit = 1;
		startAlarm( );

	}
	else if( alarm2Energy && (percentRem <= alarm2Energy) && !alarmTwoHit && !silenceAlarmTwo && !activeAlarm && !alarmToResume )
	{

		remainingSets = numSets;
		activeAlarm = 2;
		alarmTwoHit = 1;
		startAlarm( );
	}

	if( activeAlarm && (timeSecond == alarmEnd) )
	{
		alarmToResume = activeAlarm;
		activeAlarm = 0;
		_RB7 = 0;

		if( remainingSets )
		{
			startNextAlarm = (timeSecond + 59) % 60;
			remainingSets--;
		}
	}
	else if( audibleAlarm && activeAlarm && ((timeSecond % 2) == alarmPulse) )
	{
		_RB7 = 1;
	}
	else
	{
		_RB7 = 0;
	}


	if( percentRem != alarm1Energy )
		silenceAlarmOne = 0;
	if( percentRem != alarm2Energy )
		silenceAlarmTwo = 0;

	if( (startNextAlarm == timeSecond) && remainingSets )
	{
		activeAlarm = alarmToResume;
		startAlarm( );
	}
}
void startAlarm( void )
{
	if( menuState != MENU_ALARM )
		oldMenuState = menuState;
	alarmPulse = (timeSecond + 1) % 2;
	alarmEnd = (timeSecond + (2 * numBeeps)) % 60;
	menuState = MENU_ALARM;

	if( BACKLIGHT_NORMAL == true )
	{
		BACKLIGHT = 1; // turn on backlight
	}
	resetTimeSecond = (timeSecond + 59) % 60;
	resetTimeMinute = (timeMinute + 9) % 60;
}
void nextDot( void )
{
	static char count = 0;
	commDelay( 15000 );
	writeToDisplay( ".", 12 + count++, 0 );
}

//void initDisplayBox( void )
//{
//    writeToDisplay( "Initializing", 0, 0 );
//
//
//    //    com_command_readRemoteTime( );
//    //    nextDot( );
//    //    debugBacklight( true );
//    //    com_command_readRemoteEnergyAllocation( );
//    //    nextDot( );
//    //    com_command_readRemoteAlarm( );
//    //    nextDot( );
//    //    com_command_readRemotePassword( );
//    //    nextDot( );
//    //    com_command_readRemoteEmergency( );
//    //    nextDot( );
//    //    com_command_readRemoteResetTime( );
//    //    nextDot( );
//    //    com_command_readRemoteRelay( );
//    //    nextDot( );
//    //    com_command_readRemoteStat( );
//    //    nextDot( );
//    //    //    com_command_readRemoteHL( );
//    //    //    nextDot( );
//    //    com_command_readRemoteCBver( );
//    //    nextDot( );
//    //    com_command_readRemotePowerFailTimes( );
//    //    //    com_command_readRemotePowerDownUpTime( );
//    //    nextDot( );
//}
void periodicUpdate( void )
{

	static bool firstRun = true;
	static int firstRunStep = 0;
	static bool alreadyRun = false;
	// every x seconds run one of the init requests
	// when last request is run we will make it work

	if( firstRun == true )
	{
		if( (timeSecond % 2) == 0 )
		{
			if( alreadyRun == false )
			{
				alreadyRun = true;

				firstRunStep++;
				switch( firstRunStep )
				{
					case 1:
						com_command_readRemoteTime( );
						break;
					case 2:
						com_command_readRemoteEnergyAllocation( );
						break;
					case 3:
						com_command_readRemoteAlarm( );
						break;
					case 4:
						com_command_readRemotePassword( );
						break;
					case 5:
						com_command_readRemoteEmergency( );
						break;
					case 6:
						com_command_readRemoteResetTime( );
						break;
					case 7:
						com_command_readRemoteRelay( );
						break;
					case 8:
						com_command_readRemoteStat( );
						break;
					case 9:
						com_command_readRemoteCBver( );
						break;
					case 10:
						com_command_readRemotePowerFailTimes( );
						break;
					default:
						firstRun = false;
						break;
				}

			}
		}
		else
		{
			alreadyRun = false;
		}
	}
	else
	{

		static char lastPowerSecond = 0,
				lastOtherSecond = 0;

		//FIX
		// problem here is that the commands get tripped over
		// seems like the power and time get scrambled when both
		// are run at the same time
		// likely due to receiving commands and the buffer running on while it is beinf processed

		if( enablePeriodicUpdate )
		{
			// Refresh power every second
			if( timeSecond != lastPowerSecond )
			{
				com_command_readRemotePowerData( );
				lastPowerSecond = timeSecond;
			}
			// Refresh time and other settings every 10 seconds
			if( ((timeSecond % 10) == 0) && (timeSecond != lastOtherSecond) )
			{
				com_command_readRemoteTime( );
				readTime( );
				lastOtherSecond = timeSecond;
			}
		}
		else
		{
			// Keep checking, but only every 10 seconds
			if( timeSecond % 10 == 0 && timeSecond != lastPowerSecond )
			{
				com_command_readRemotePowerData( );
				lastPowerSecond = timeSecond;
			}
		}

		if( timeSecond % 60 == 0 )
		{
			// here we should grab all or most of the parameters so that we keep up to date

		}
	}
}

//void checkCommFailure( void )
//{
//    if( commError > 90 )
//    {
//	// no communication received for 1 minute
//	// display a message and reboot
//	if( BACKLIGHT_NORMAL == true )
//	{
//	    BACKLIGHT = 1; // turn on backlight
//	}
//	writeToDisplay( "System will restart in 15 seconds due toa communication linkproblem.", 0, 80 );
//	for(;; );
//    }
//
//    if( timeSecond != commErrorTime )
//    {
//	commError++;
//	commErrorTime = timeSecond;
//    }
//}

// ignore red error marks (caused by some failure regarding macro definition)
void initRTCCDisplay( void )
{

	//does unlock sequence to enable write to RTCC, sets RTCWEN
	__builtin_write_RTCWEN( );

	RCFGCAL = 0b0010001100000000;
	RTCPWC = 0b0000010100000000; // LPRC is clock source

	_RTCEN = 1;

	_RTCWREN = 0; // Disable Writing
}
void initTimer( void )
{

	// timer 1 is for LCD display
	T1CON = 0x8030; // timer 1 setting
	PR1 = 0x0500; // 0x2000 is approximately a second
	//IEC0 = 0b0000000000001000;
	// 0x0094 is file IEC0. Bit 3 controls timer1 interrupts
	TMR1 = 0;
	_T1IE = 1;
	_T1IF = 0;

	// timer 2 is for button press detection
	T2CON = 0x8030; // timer 2 setting
	PR2 = 0x0100; // 0x2000 is approximately a second
	//IEC0 = 0b0000000000001000;
	// 0x0094 is file IEC0. Bit 3 controls timer2 interrupts
	TMR2 = 0;
	_T2IE = 1;
	_T2IF = 0;
}
/* writeClockStrings
 * writes clock and calendar strings for output to LCD screen
 * current formats:
 *     clock: "HH:MM"
 *     calendar: "DD/MM/YY"
 */
void writeClockStrings( void )
{

	calendarStr[0] = (timeDay / 10) + 0x30;
	calendarStr[1] = (timeDay % 10) + 0x30;
	calendarStr[2] = calendarStr[5] = '/';
	calendarStr[3] = (timeMonth / 10) + 0x30;
	calendarStr[4] = (timeMonth % 10) + 0x30;
	calendarStr[6] = (timeYear / 10) + 0x30;
	calendarStr[7] = (timeYear % 10) + 0x30;

	clockStr[0] = (timeHour / 10) + 0x30;
	clockStr[1] = (timeHour % 10) + 0x30;
	clockStr[2] = ':';
	clockStr[3] = (timeMinute / 10) + 0x30;
	clockStr[4] = (timeMinute % 10) + 0x30;


	calendarStr[8] = clockStr[5] = 0;
}
void writeTempClockStrings( void )
{
	tempClockStr[0] = tempClockStr[3] = tempClockStr[5] = tempClockStr[8] = tempCalStr[0] = tempCalStr[3] = tempCalStr[5] = tempCalStr[8] = tempCalStr[10] = tempCalStr[13] = ' ';
	tempClockStr[4] = ':';
	tempCalStr[4] = tempCalStr[9] = '/';
	tempClockStr[9] = tempCalStr[14] = 0;

	tempClockStr[1] = (tempHour / 10) + 0x30;
	tempClockStr[2] = (tempHour % 10) + 0x30;
	tempClockStr[6] = (tempMin / 10) + 0x30;
	tempClockStr[7] = (tempMin % 10) + 0x30;
	tempCalStr[1] = (tempDay / 10) + 0x30;
	tempCalStr[2] = (tempDay % 10) + 0x30;
	tempCalStr[6] = (tempMonth / 10) + 0x30;
	tempCalStr[7] = (tempMonth % 10) + 0x30;
	tempCalStr[11] = (tempYear / 10) + 0x30;
	tempCalStr[12] = (tempYear % 10) + 0x30;

	switch( timeSetPos )
	{
		case 1:
			tempClockStr[0] = 0x7E;
			tempClockStr[3] = 0x7F;
			break;

		case 2:
			tempClockStr[5] = 0x7E;
			tempClockStr[8] = 0x7F;
			break;

		case 3:
			tempCalStr[0] = 0x7E;
			tempCalStr[3] = 0x7F;
			break;

		case 4:
			tempCalStr[5] = 0x7E;
			tempCalStr[8] = 0x7F;
			break;

		case 5:
			tempCalStr[10] = 0x7E;
			tempCalStr[13] = 0x7F;
	}
}

/* Interrupts *****************************************************************/
/* Timer 2 Interrupt
 * Detects button presses
 */
void __attribute__( (interrupt, no_auto_psv) ) _T2Interrupt( void )
{

	// clear flag in main loop code by setting to a value other than 0 or 1
	// pressed = 1
	// pressed and resolved = 2?
	// released = 0
	if( BACKLIGHT )
	{
		// _RA4 is pin 10
		if( !BTN_3 )
			button3Flag = 0;
		else if( !button3Flag )
		{// && _RA3 == 1
			button3Flag = 1;
			resetTimeSecond = (timeSecond + 59) % 60;
			resetTimeMinute = (timeMinute + 9) % 60;
		}

		// _RB4 is pin 9
		if( !BTN_2 )
			button2Flag = 0;
		else if( !button2Flag )
		{// && _RB8 == 1
			button2Flag = 1;
			resetTimeSecond = (timeSecond + 59) % 60;
			resetTimeMinute = (timeMinute + 9) % 60;
		}

		// _RA3 is pin 8
		if( !BTN_1 )
			button1Flag = 0;
		else if( !button1Flag )
		{// && _RA4 == 1
			button1Flag = 1;
			resetTimeSecond = (timeSecond + 59) % 60;
			resetTimeMinute = (timeMinute + 9) % 60;
		}

		// _RA2 is pin 7
		if( !BTN_0 )
			button0Flag = 0;
		else if( !button0Flag )
		{// && _RB4 == 1
			button0Flag = 1;
			resetTimeSecond = (timeSecond + 59) % 60;
			resetTimeMinute = (timeMinute + 9) % 60;
		}
	}

	else if( BTN_0 || BTN_1 || BTN_2 || BTN_3 )
	{
		if( BACKLIGHT_NORMAL == true )
		{
			BACKLIGHT = 1; // turn on backlight
		}
		button0Flag = 2;
		button1Flag = 2;
		button2Flag = 2;
		button3Flag = 2;
		resetTimeSecond = (timeSecond + 59) % 60;
		resetTimeMinute = (timeMinute + 9) % 60;
	}

	if( (timeMinute == resetTimeMinute) && (timeSecond == resetTimeSecond) && ((menuState != MENU_ALARM) || ((menuState == MENU_ALARM) && (remainingSets == 0))) && (menuState != MENU_DEBUG) && (!isBooting) )
	{
		menuState = MENU_HOME_BASIC;
		if( BACKLIGHT_NORMAL == true )
		{
			BACKLIGHT = 0; // turn on backlight
		}
	}

	_T2IF = 0; // clear interrupt flag
}





//
void debugBacklightToggle( )
{
	if( BACKLIGHT_NORMAL == false )
	{
		if( BACKLIGHT == 1 )
		{
			BACKLIGHT = 0;
		}
		else
		{
			BACKLIGHT = 1;
		}
	}

	return;
}


void debugBacklight( bool state )
{
	if( BACKLIGHT_NORMAL == false )
	{
		if( state == true )
		{
			BACKLIGHT = 1;
		}
		else
		{
			BACKLIGHT = 0;
		}
	}

	return;
}
