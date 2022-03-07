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


char alarmEnd_module;
char alarmPulse_module;

char numBeeps_module;
char numSets_module;


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
void debugBacklightFlash( int timeOn );


/****************
 CODE
 ****************/

/* main
 * Initializes and runs through the main code that is repetitively called
 */
int main( void )
{
	resetTimeSecond_global = 59;
	resetTimeMinute_global = 9;
	resetTimeHour_global = 8;

	init( );

	communications( true );
	static unsigned char lowPriorityCounter = 0;

	//    initDisplayBox( );

	enablePeriodicUpdate_global = 1;

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
	tempHour_global = timeHour_global;
	tempMin_global = timeMinute_global;
	tempMonth_global = timeMonth_global;
	tempDay_global = timeDay_global;
	tempYear_global = timeYear_global;

	initVars( );
}

/* initVars
 * Initializes variables to their starting values (usually 0)
 */
void initVars( void )
{

	if( menuState_global != MENU_DEBUG )
		menuState_global = MENU_HOME_BASIC;

	//FIX DONE
	//    stringCopy( "Unknown ", powerBoxCodeVersionString );

	powerBoxCodeVersionString_global[0] = 'U';
	powerBoxCodeVersionString_global[1] = 'n';
	powerBoxCodeVersionString_global[2] = 'k';
	powerBoxCodeVersionString_global[3] = 'n';
	powerBoxCodeVersionString_global[4] = 'o';
	powerBoxCodeVersionString_global[5] = 'w';
	powerBoxCodeVersionString_global[6] = 'n';
	powerBoxCodeVersionString_global[7] = ' ';
	powerBoxCodeVersionString_global[8] = CHAR_NULL;


	audibleAlarm_global = 0;
	alarm1Enabled_global = 0;
	alarm2Enabled_global = 0;
	alarm1Energy_global = 0;
	alarm2Energy_global = 0;
	activeAlarm_global = 0;
	numBeeps_module = 4;
	numSets_module = 4;
	tempPercent_global = 0;
	emerButtonEnable_global = 1;
	emerButtonEnergyAllocate_global = 250;
	emerAllocNow_global = 50;
	emerAllocSend_global = 0;

	resetTimeHour_global = 0;
	resetTimeMinute_global = 0;
	tempResetHour_global = 0;
	tempResetMinute_global = 0;

	relayMode_global = 0;  // default to show relay is in off mode
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

	if( isBooting_global ) return;

	if( percentRem_global > 95 )
	{
		alarmOneHit_global = alarmTwoHit_global = 0;
	}

	if( alarm1Energy_global && ( percentRem_global <= alarm1Energy_global ) && !alarmOneHit_global && !silenceAlarmOne_global && !activeAlarm_global && !alarmToResume_global )
	{

		remainingSets_global = numSets_module;
		activeAlarm_global = 1;
		alarmOneHit_global = 1;
		startAlarm( );

	}
	else if( alarm2Energy_global && ( percentRem_global <= alarm2Energy_global ) && !alarmTwoHit_global && !silenceAlarmTwo_global && !activeAlarm_global && !alarmToResume_global )
	{

		remainingSets_global = numSets_module;
		activeAlarm_global = 2;
		alarmTwoHit_global = 1;
		startAlarm( );
	}

	if( activeAlarm_global && ( timeSecond_global == alarmEnd_module ) )
	{
		alarmToResume_global = activeAlarm_global;
		activeAlarm_global = 0;
		_RB7 = 0;

		if( remainingSets_global )
		{
			startNextAlarm = ( timeSecond_global + 59 ) % 60;
			remainingSets_global--;
		}
	}
	else if( audibleAlarm_global && activeAlarm_global && ( ( timeSecond_global % 2 ) == alarmPulse_module ) )
	{
		_RB7 = 1;
	}
	else
	{
		_RB7 = 0;
	}


	if( percentRem_global != alarm1Energy_global )
		silenceAlarmOne_global = 0;
	if( percentRem_global != alarm2Energy_global )
		silenceAlarmTwo_global = 0;

	if( ( startNextAlarm == timeSecond_global ) && remainingSets_global )
	{
		activeAlarm_global = alarmToResume_global;
		startAlarm( );
	}
}

void startAlarm( void )
{
	if( menuState_global != MENU_ALARM )
		oldMenuState_global = menuState_global;
	alarmPulse_module = ( timeSecond_global + 1 ) % 2;
	alarmEnd_module = ( timeSecond_global + ( 2 * numBeeps_module ) ) % 60;
	menuState_global = MENU_ALARM;

	if( BACKLIGHT_NORMAL == true )
	{
		BACKLIGHT = 1; // turn on backlight
	}
	resetTimeSecond_global = ( timeSecond_global + 59 ) % 60;
	resetTimeMinute_global = ( timeMinute_global + 9 ) % 60;
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
		if( ( timeSecond_global % 2 ) == 0 )
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

		if( enablePeriodicUpdate_global )
		{
			// Refresh power every second
			if( timeSecond_global != lastPowerSecond )
			{
				com_command_readRemotePowerData( );
				lastPowerSecond = timeSecond_global;
			}
			// Refresh time and other settings every 10 seconds
			if( ( ( timeSecond_global % 10 ) == 0 ) && ( timeSecond_global != lastOtherSecond ) )
			{
				com_command_readRemoteTime( );
				readTime( );
				lastOtherSecond = timeSecond_global;
			}
		}
		else
		{
			// Keep checking, but only every 10 seconds
			if( timeSecond_global % 10 == 0 && timeSecond_global != lastPowerSecond )
			{
				com_command_readRemotePowerData( );
				lastPowerSecond = timeSecond_global;
			}
		}

		if( timeSecond_global % 60 == 0 )
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

	calendarStr_global[0] = ( timeDay_global / 10 ) + 0x30;
	calendarStr_global[1] = ( timeDay_global % 10 ) + 0x30;
	calendarStr_global[2] = calendarStr_global[5] = '/';
	calendarStr_global[3] = ( timeMonth_global / 10 ) + 0x30;
	calendarStr_global[4] = ( timeMonth_global % 10 ) + 0x30;
	calendarStr_global[6] = ( timeYear_global / 10 ) + 0x30;
	calendarStr_global[7] = ( timeYear_global % 10 ) + 0x30;

	clockStr_global[0] = ( timeHour_global / 10 ) + 0x30;
	clockStr_global[1] = ( timeHour_global % 10 ) + 0x30;
	clockStr_global[2] = ':';
	clockStr_global[3] = ( timeMinute_global / 10 ) + 0x30;
	clockStr_global[4] = ( timeMinute_global % 10 ) + 0x30;


	calendarStr_global[8] = clockStr_global[5] = 0;
}

void writeTempClockStrings( void )
{
	tempClockStr_global[0] = tempClockStr_global[3] = tempClockStr_global[5] = tempClockStr_global[8] = tempCalStr_global[0] = tempCalStr_global[3] = tempCalStr_global[5] = tempCalStr_global[8] = tempCalStr_global[10] = tempCalStr_global[13] = ' ';
	tempClockStr_global[4] = ':';
	tempCalStr_global[4] = tempCalStr_global[9] = '/';
	tempClockStr_global[9] = tempCalStr_global[14] = 0;

	tempClockStr_global[1] = ( tempHour_global / 10 ) + 0x30;
	tempClockStr_global[2] = ( tempHour_global % 10 ) + 0x30;
	tempClockStr_global[6] = ( tempMin_global / 10 ) + 0x30;
	tempClockStr_global[7] = ( tempMin_global % 10 ) + 0x30;
	tempCalStr_global[1] = ( tempDay_global / 10 ) + 0x30;
	tempCalStr_global[2] = ( tempDay_global % 10 ) + 0x30;
	tempCalStr_global[6] = ( tempMonth_global / 10 ) + 0x30;
	tempCalStr_global[7] = ( tempMonth_global % 10 ) + 0x30;
	tempCalStr_global[11] = ( tempYear_global / 10 ) + 0x30;
	tempCalStr_global[12] = ( tempYear_global % 10 ) + 0x30;

	switch( timeSetPos_global )
	{
		case 1:
			tempClockStr_global[0] = 0x7E;
			tempClockStr_global[3] = 0x7F;
			break;

		case 2:
			tempClockStr_global[5] = 0x7E;
			tempClockStr_global[8] = 0x7F;
			break;

		case 3:
			tempCalStr_global[0] = 0x7E;
			tempCalStr_global[3] = 0x7F;
			break;

		case 4:
			tempCalStr_global[5] = 0x7E;
			tempCalStr_global[8] = 0x7F;
			break;

		case 5:
			tempCalStr_global[10] = 0x7E;
			tempCalStr_global[13] = 0x7F;
	}
}

/* Interrupts *****************************************************************/

/* Timer 2 Interrupt
 * Detects button presses
 */
void __attribute__( ( interrupt, no_auto_psv ) ) _T2Interrupt( void )
{

	// clear flag in main loop code by setting to a value other than 0 or 1
	// pressed = 1
	// pressed and resolved = 2?
	// released = 0
	if( ( BACKLIGHT == true ) || ( BACKLIGHT_NORMAL == false ) )
	{
		// _RA4 is pin 10
		if( !BTN_3 )
			button3Flag_global = 0;
		else if( !button3Flag_global )
		{// && _RA3 == 1
			button3Flag_global = 1;
			resetTimeSecond_global = ( timeSecond_global + 59 ) % 60;
			resetTimeMinute_global = ( timeMinute_global + 9 ) % 60;
		}

		// _RB4 is pin 9
		if( !BTN_2 )
			button2Flag_global = 0;
		else if( !button2Flag_global )
		{// && _RB8 == 1
			button2Flag_global = 1;
			resetTimeSecond_global = ( timeSecond_global + 59 ) % 60;
			resetTimeMinute_global = ( timeMinute_global + 9 ) % 60;
		}

		// _RA3 is pin 8
		if( !BTN_1 )
			button1Flag_global = 0;
		else if( !button1Flag_global )
		{// && _RA4 == 1
			button1Flag_global = 1;
			resetTimeSecond_global = ( timeSecond_global + 59 ) % 60;
			resetTimeMinute_global = ( timeMinute_global + 9 ) % 60;
		}

		// _RA2 is pin 7
		if( !BTN_0 )
			button0Flag_global = 0;
		else if( !button0Flag_global )
		{// && _RB4 == 1
			button0Flag_global = 1;
			resetTimeSecond_global = ( timeSecond_global + 59 ) % 60;
			resetTimeMinute_global = ( timeMinute_global + 9 ) % 60;
		}
	}

	else if( BTN_0 || BTN_1 || BTN_2 || BTN_3 )
	{
		if( BACKLIGHT_NORMAL == true )
		{
			BACKLIGHT = 1; // turn on backlight
		}
		button0Flag_global = 2;
		button1Flag_global = 2;
		button2Flag_global = 2;
		button3Flag_global = 2;
		resetTimeSecond_global = ( timeSecond_global + 59 ) % 60;
		resetTimeMinute_global = ( timeMinute_global + 9 ) % 60;
	}

	if( ( timeMinute_global == resetTimeMinute_global ) && ( timeSecond_global == resetTimeSecond_global ) && ( ( menuState_global != MENU_ALARM ) || ( ( menuState_global == MENU_ALARM ) && ( remainingSets_global == 0 ) ) ) && ( menuState_global != MENU_DEBUG ) && ( !isBooting_global ) )
	{
		menuState_global = MENU_HOME_BASIC;
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

void debugBacklightFlash( int timeOn )
{
	if( BACKLIGHT_NORMAL == false )
	{
		BACKLIGHT = 1;
		delayMS( timeOn );
		BACKLIGHT = 0;
	}

	return;
}