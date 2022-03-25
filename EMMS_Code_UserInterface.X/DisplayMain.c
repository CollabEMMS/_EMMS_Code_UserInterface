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

/****************
 MACROS
 ****************/
#define BTN_0       _RA2    // Pin 7:  RA2
#define BTN_1       _RA3    // Pin 8:  RA3
#define BTN_2       _RB4    // Pin 9:  RB4
#define BTN_3       _RA4    // Pin 10: RA4

#define BUZZER_PIN	_RB7

// Time window where the oneshots can occur
// sometimes the program main loop takes more than 1 ms to run and we can inadvertantly skip if we are lookng for an exact match
#define ONESHOT_WINDOW 25

#define BACKLIGHT_TIMEOUT_MINUTES	5
#define MENU_TIMEOUT_SECONDS		60


#define ALARM_1_TIMEOUT_SECONDS		60		// length of time for alarm to go off in seconds	
#define ALARM_1_ON_TIME_MS			100
#define ALARM_1_OFF_TIME_MS			750

#define ALARM_2_TIMEOUT_SECONDS		60		// length of time for alarm to go off in seconds	
#define ALARM_2_ON_TIME_MS			250
#define ALARM_2_OFF_TIME_MS			250


/****************
 VARIABLES
 these are the globals required by only this c file
 there should be as few of these as possible to help keep things clean
 variables required by other c functions should be here and also in the header .h file
 as external
 ****************/
// external

struct moduleInfo_struct moduleInfo_global[MODULE_COUNT];

// internal only

volatile unsigned long msTimer_module; // toggles to 1 every ms, resets to 0 at about the 0.5ms time

bool alarmSilence_global;

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
void initOscillator( void );

void initTimer( void );
void initPorts( void );
void enablePullDownResistors( void );


void enableInterrupts( void );

void initVars( void );
void periodicDataUpdate( void );

void periodicUpdate( void );
void checkAlarm( void );

void dispButtonPress( void );

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

	initOscillator( );

	init( );
	initVars( );


	communications( true );

	enablePeriodicUpdate_global = true;

	while( true )
	{

		// oneShot
		{
			static bool oneShot = false;

			if( ( msTimer_module % 200 ) <= ONESHOT_WINDOW )
			{
				if( oneShot == false )
				{
					oneShot = true;
					dispUpdate( );
				}
			}
			else
			{
				oneShot = false;
			}
		}

		// oneShot
		{
			static bool oneShot = false;

			if( ( msTimer_module % 50 ) <= ONESHOT_WINDOW )
			{
				if( oneShot == false )
				{
					oneShot = true;
					dispButtonPress( );
				}
			}
			else
			{
				oneShot = false;
			}
		}

		// oneShot
		{
			static bool oneShot = false;

			if( ( msTimer_module % 50 ) <= ONESHOT_WINDOW )
			{
				if( oneShot == false )
				{
					oneShot = true;
					periodicDataUpdate( );
				}
			}
			else
			{
				oneShot = false;
			}
		}


		// oneShot
		{
			static bool oneShot = false;

			if( ( msTimer_module % 200 ) <= ONESHOT_WINDOW )
			{
				if( oneShot == false )
				{
					oneShot = true;
					rtccReadTime( );
					calcPercentRem( );
					calcTimeRemaining( );
				}
			}
			else
			{
				oneShot = false;
			}
		}

		checkAlarm( );
		updateMenu( );
		communications( false );
	}
}

/* Functions ******************************************************************/

/* init_defaults
 Initializes default values into variables
 */

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
	enableInterrupts( );
	rtccReadTime( );

	tempHour_global = timeHour_global;
	tempMin_global = timeMinute_global;
	tempMonth_global = timeMonth_global;
	tempDay_global = timeDay_global;
	tempYear_global = timeYear_global;

	return;
}

/* initOscillator
 * Set oscillator to fastest 16MHz
 */
void initOscillator( void )
{
	// set clock to fastest available - 16MHz

	// unlock the OSCON high byte
	OSCCON = 0x78; //0x78 = 0111 1000
	OSCCON = 0x9A; // 0x9A = 1001 1010
	// set bits in the OSCCON high byte
	OSCCONbits.NOSC = 0b001; // set fast RC OSC


	// unlock sequence for the OSCCON low byte
	OSCCON = 0x46; // 0x46 = 0100 0110
	OSCCON = 0x57; // 0x58 = 0101 0111
	// set bits in the OSCCON low byte
	// set the clock switch bit - this must be done immediately after the unlock sequence
	OSCCONbits.OSWEN = 1;

	// set clock divider to 1:1
	CLKDIVbits.RCDIV = 0b000;

	return;
}

/* initVars
 * Initializes variables to their starting values (usually 0)
 */
void initVars( void )
{
	powerDownTime_global[0] = ' ';
	powerDownTime_global[1] = ' ';
	powerDownTime_global[2] = 'N';
	powerDownTime_global[3] = 'o';
	powerDownTime_global[4] = ' ';
	powerDownTime_global[5] = 'D';
	powerDownTime_global[6] = 'a';
	powerDownTime_global[7] = 't';
	powerDownTime_global[8] = 'a';
	powerDownTime_global[9] = ' ';
	powerDownTime_global[10] = ' ';
	powerDownTime_global[11] = CHAR_NULL;

	powerUpTime_global[0] = ' ';
	powerUpTime_global[1] = ' ';
	powerUpTime_global[2] = 'N';
	powerUpTime_global[3] = 'o';
	powerUpTime_global[4] = ' ';
	powerUpTime_global[5] = 'D';
	powerUpTime_global[6] = 'a';
	powerUpTime_global[7] = 't';
	powerUpTime_global[8] = 'a';
	powerUpTime_global[9] = ' ';
	powerUpTime_global[10] = ' ';
	powerUpTime_global[11] = CHAR_NULL;

	if( menuState_global != MENU_DEBUG )
	{
		menuState_global = MENU_HOME_BASIC;
	}

	meterNameString_global[0] = 'N';
	meterNameString_global[1] = 'o';
	meterNameString_global[2] = ' ';
	meterNameString_global[3] = 'D';
	meterNameString_global[4] = 'a';
	meterNameString_global[5] = 't';
	meterNameString_global[6] = 'a';
	meterNameString_global[7] = ' ';
	meterNameString_global[8] = CHAR_NULL;


	alarmAudible_global = false;
	alarm1PercentThreshold_global = 0;
	alarm2PercentThreshold_global = 0;
	tempMenuAlarmPercent_global = 0;
	emerButtonEnable_global = 1;
	emerButtonEnergyAllocate_global = 250;
	emerAllocNow_global = 50;
	emerAllocSend_global = 0;

	resetTimeHour_global = 0;
	resetTimeMinute_global = 0;
	tempResetHour_global = 0;
	tempResetMinute_global = 0;

	relayMode_global = 0; // default to show relay is in off mode

	for( int inx = 0; inx < MODULE_COUNT; inx++ )
	{
		for( int jnx = 0; jnx < MODULE_INFO_SIZE_SMALL; jnx++ )
		{
			moduleInfo_global[inx].info0[jnx] = CHAR_NULL;
			moduleInfo_global[inx].info1[jnx] = CHAR_NULL;
			moduleInfo_global[inx].info2[jnx] = CHAR_NULL;
			moduleInfo_global[inx].info3[jnx] = CHAR_NULL;
			moduleInfo_global[inx].info4[jnx] = CHAR_NULL;
		}

		for( int jnx = MODULE_INFO_SIZE_SMALL; jnx < MODULE_INFO_SIZE_LARGE; jnx++ )
		{
			moduleInfo_global[inx].info4[jnx] = CHAR_NULL;
		}

		moduleInfo_global[inx].info0[0] = 'N';
		moduleInfo_global[inx].info0[1] = 'o';
		moduleInfo_global[inx].info0[2] = 'n';
		moduleInfo_global[inx].info0[3] = 'e';
		moduleInfo_global[inx].info0[4] = '-';
		moduleInfo_global[inx].info0[5] = inx + 48;
	}

	return;
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

	return;
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

	return;
}

/* enableInterrupts
 * as named
 */
void enableInterrupts( void )
{
	INTCON1 |= 0b1000000000000000;

	return;
}

/* disableInterrupts
 * also as named
 */
void disableInterrupts( void )
{
	INTCON1 &= 0b0111111111111111;

	return;
}

void checkAlarm( void )
{

	static bool alarm1Active = false;
	static unsigned char alarm1EndTimeSecond = 0;
	static unsigned char alarm1EndTimeMinute = 0;
	static unsigned long alarm1TimeTurnOn = 0;
	static unsigned long alarm1TimeTurnOff = 0;
	static bool alarm1Silence = false;

	static bool alarm2Active = false;
	static unsigned char alarm2EndTimeSecond = 0;
	static unsigned char alarm2EndTimeMinute = 0;
	static unsigned long alarm2TimeTurnOn = 0;
	static unsigned long alarm2TimeTurnOff = 0;
	static bool alarm2Silence = false;

	if( alarm1PercentThreshold_global > 0 )
	{
		if( ( alarm2Active == false ) || ( alarm2Silence == true ) )
		{
			if( ( percentRem_global <= alarm1PercentThreshold_global ) )
			{
				if( alarm1Active == false )
				{
					alarm1Active = true;
					alarm1EndTimeSecond = timeSecond_global + ALARM_1_TIMEOUT_SECONDS;
					alarm1EndTimeMinute = timeMinute_global;
					while( alarm1EndTimeSecond >= 60 )
					{
						alarm1EndTimeMinute++;
						alarm1EndTimeSecond -= 60;
					}

					menuState_global = MENU_ALARM;
					if( BACKLIGHT_NORMAL == true )
					{
						BACKLIGHT = 1; // turn on backlight
					}
				} //	if( alarm1Active == false )
				else
				{
					if( alarm1Silence == false )
					{
						if( ( ( timeMinute_global == alarm1EndTimeMinute ) && ( timeSecond_global >= alarm1EndTimeSecond ) ) || ( alarmSilence_global == true ) )
						{
							alarmSilence_global = false;
							alarm1Silence = true;
							alarm1TimeTurnOn = 0;
							alarm1TimeTurnOff = 0;
							menuState_global = MENU_HOME_BASIC;
						}
					}
				} // else --				if( alarm1Active == false )
			} //		if( ( percentRem_global <= alarm1PercentThreshold_global )
			else
			{
				if( ( alarm1Active == true ) && ( alarm1Silence == false ) )
				{
					menuState_global = MENU_HOME_BASIC;
				}
				alarm1Active = false;
				alarm1Silence = false;
			} //	else ---	if( ( percentRem_global <= alarm1PercentThreshold_global )

			if( alarm1Active == true )
			{
				if( ( alarm1Silence == true ) || ( alarmAudible_global == false ) )
				{
					BUZZER_PIN = 0;
				}
				else
				{
					// oneShot
					{
						static bool oneShot = false;

						if( msTimer_module > alarm1TimeTurnOn )
						{
							if( oneShot == false )
							{
								oneShot = true;
								alarm1TimeTurnOff = msTimer_module + ALARM_1_ON_TIME_MS;
								BUZZER_PIN = 1;
							}
						}
						else
						{
							oneShot = false;
						}
					} // oneShot

					// oneShot
					{
						static bool oneShot = false;

						if( msTimer_module > alarm1TimeTurnOff )
						{
							if( oneShot == false )
							{
								oneShot = true;
								alarm1TimeTurnOn = msTimer_module + ALARM_1_OFF_TIME_MS;
								BUZZER_PIN = 0;
							}
						}
						else
						{
							oneShot = false;
						}
					} // oneShot
				}
			} //			if( alarm1Active == true )
		} // 			if( ( alarm2Active == false ) || ( ( alarm2Active == true ) && ( alarm2Silence == true ) ) )
	} //		if( alarm1PercentThreshold_global > 0 )


	if( alarm2PercentThreshold_global > 0 )
	{
		if( ( alarm1Active == false ) || ( alarm1Silence == true ) )
		{
			if( ( percentRem_global <= alarm2PercentThreshold_global ) )
			{
				if( alarm2Active == false )
				{
					alarm2Active = true;
					alarm2EndTimeSecond = timeSecond_global + ALARM_2_TIMEOUT_SECONDS;
					alarm2EndTimeMinute = timeMinute_global;
					while( alarm2EndTimeSecond >= 60 )
					{
						alarm2EndTimeMinute++;
						alarm2EndTimeSecond -= 60;
					}

					menuState_global = MENU_ALARM;
					if( BACKLIGHT_NORMAL == true )
					{
						BACKLIGHT = 1; // turn on backlight
					}
				} //	if( alarm2Active == false )
				else
				{
					if( alarm2Silence == false )
					{
						if( ( ( timeMinute_global == alarm2EndTimeMinute ) && ( timeSecond_global >= alarm2EndTimeSecond ) ) || ( alarmSilence_global == true ) )
						{
							alarmSilence_global = false;
							alarm2Silence = true;
							alarm2TimeTurnOn = 0;
							alarm2TimeTurnOff = 0;
							menuState_global = MENU_HOME_BASIC;
						}
					}
				} // else --				if( alarm2Active == false )
			} //		if( ( percentRem_global <= alarm2PercentThreshold_global )
			else
			{
				if( ( alarm2Active == true ) && ( alarm2Silence == false ) )
				{
					menuState_global = MENU_HOME_BASIC;
				}
				alarm2Active = false;
				alarm2Silence = false;
			} //	else ---	if( ( percentRem_global <= alarm2PercentThreshold_global )

			if( alarm2Active == true )
			{
				if( ( alarm2Silence == true ) || ( alarmAudible_global == false ) )
				{
					BUZZER_PIN = 0;
				}
				else
				{
					// oneShot
					{
						static bool oneShot = false;

						if( msTimer_module > alarm2TimeTurnOn )
						{
							if( oneShot == false )
							{
								oneShot = true;
								alarm2TimeTurnOff = msTimer_module + ALARM_2_ON_TIME_MS;
								BUZZER_PIN = 1;
							}
						}
						else
						{
							oneShot = false;
						}
					} // oneShot

					// oneShot
					{
						static bool oneShot = false;

						if( msTimer_module > alarm2TimeTurnOff )
						{
							if( oneShot == false )
							{
								oneShot = true;
								alarm2TimeTurnOn = msTimer_module + ALARM_2_OFF_TIME_MS;
								BUZZER_PIN = 0;
							}
						}
						else
						{
							oneShot = false;
						}
					} // oneShot
				}
			} //			if( alarm2Active == true )
		} // 			if( ( alarm1Active == false ) || ( ( alarm1Active == true ) && ( alarm1Silence == true ) ) )
	} //		if( alarm2PercentThreshold_global > 0 )

	// make sure the buzzer is turned off if nothing causes it to be on
	if(
	 (
	 ( ( alarm1Active == false ) || ( alarm1Silence == true ) )
	 && ( ( alarm2Active == false ) || ( alarm2Silence == true ) )
	 )
	 || ( alarmAudible_global == false )
	 )
	{
		BUZZER_PIN = 0;
	}

	return;
}

void periodicDataUpdate( void )
{
	// each time this is called we should ask for a new data item.
	// data to read
	//		ITEM					FREQUENCY
	//*		statistics				1
	//*		module info				1
	//*		power fail times		1
	//*		date/time				2
	//*		energy allocation		2
	//*		alarm info				2
	//*		password				2
	//*		emergency button		2
	//*		reset time				2
	//*		relay control			3
	//*		Power Data				3

	static int messageCounterMain = 0;
	static int messageCounterRateLow = 0;
	static int messageCounterRateMedium = 0;
	static int messageCounterRateHigh = 0;

	if( commBufferEmpty( ) == true )
	{
		messageCounterMain++;
		if( messageCounterMain >= 1000 )
		{
			messageCounterMain = 0;
		}

		// send some messages at a slower rate
		// this allows the higher priority messages to be sent more often

		if( ( messageCounterMain % 10 ) == 0 ) // every 10th message
		{
			messageCounterRateLow++;

			switch( messageCounterRateLow )
			{
				case 0:
					// never happens
					break;
				case 1:
					com_command_readRemoteStat( );
					break;
				case 2:
					com_command_readRemotePowerFailTimes( );
					break;
				case 3:
					com_command_readRemoteMeterName( );
					break;
				case 4:
					com_command_sendModuleInfoThis( );
				default:
					messageCounterRateLow = 0;
			}
		}
		else if( ( messageCounterMain % 5 ) == 0 )
		{
			if( enablePeriodicUpdate_global == true )
			{
				messageCounterRateMedium++;

				switch( messageCounterRateMedium )
				{
					case 0:
						// never happens
						break;
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
						com_command_readModuleInfo( );
						break;

					default:
						messageCounterRateMedium = 0;
				}
			}
		}
		else
		{
			messageCounterRateHigh++;

			switch( messageCounterRateHigh )
			{

				case 0:
					// never happens
					break;
				case 1:
					com_command_readRemotePowerData( );
					break;
				default:
					messageCounterRateHigh = 0;
			}
		}
	}

	return;
}

void initRTCCDisplay( void )
{
	//does unlock sequence to enable write to RTCC, sets RTCWEN

	__builtin_write_RTCWEN( );

	RCFGCAL = 0b0010001100000000;
	RTCPWC = 0b0000010100000000; // LPRC is clock source

	_RTCEN = 1;

	_RTCWREN = 0; // Disable Writing

	return;
}

void initTimer( void )
{

	//	// timer 1 is for LCD display
	//	T1CON = 0x8030; // timer 1 setting  // 1000 0000  0011 0000
	//	PR1 = 0x0500; // 0x2000 is approximately a second
	//	//IEC0 = 0b0000000000001000;
	//	// 0x0094 is file IEC0. Bit 3 controls timer1 interrupts
	//	TMR1 = 0;
	//	_T1IE = 1;
	//	_T1IF = 0;

	IEC0bits.T1IE = 0; // disable timer 1 interrupt
	T1CON = 0; // disable and reset timer to known state

	//	msTimer_module = 0;

	T1CONbits.TCS = 0; // use the instruction cycle clock
	T1CONbits.T1ECS = 0b00; // use LPRC and clock source
	T1CONbits.TGATE = 0; // disable gated timer
	T1CONbits.TCKPS = 0b00; // select 1:256 prescalar
	T1CONbits.TSYNC = 0; // sync external clock input
	TMR1 = 0; // clear the timer register

	// set timer to tick at predictable rate using MACRO

	PR1 = ( FCY / 1000 ) - 1; // interrupt at 1ms interval (note the -1, this is to )
	//	PR1 = 10416U; // interrupt at 1ms interval (note the -1, this is to )
	// always load TIME -1 to make timer more accurate - one count is added due to timer internals
	//  16Mhz = 16,000,000   / 1000   = 16,000.  If at 16Mhz we interval every 16,000 clock cycles we get 1ms timer

	IPC0bits.T1IP = 0x04; // interrupt priority level
	IFS0bits.T1IF = 0; // clear timer interrupt flag
	IEC0bits.T1IE = 1; // enable interrupt
	T1CONbits.TON = 1; // enable timer

	return;
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

	return;
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

	return;
}

/* Interrupts *****************************************************************/

/* Timer 1 Interrupt
 * Updates the display with changes made to the nextDisplay array
 * Updates currentDisplay array with what is now on the display
 * Only changes lines that are different between nextDisplay and currentDisplay
 */
void __attribute__( ( interrupt, no_auto_psv ) ) _T1Interrupt( void )
{
	TMR1 = 0; // reset the accumulator
	IFS0bits.T1IF = 0; // reset the interrupt flag

	// timer is designed to interrupt at 0.001s (1ms)

	// the following variable must be declared as 'volatile'
	// this means the value can change unexpectedly, even in the middle
	// of an operation with the variable
	// because this interrupt can trigger at any time
	msTimer_module++; // increment every 1ms

	// control our timer rollover to prevent overflow
	// not critical that we do this, but it is more controlled than an overflow
	if( msTimer_module >= 4000000000 )
	{

		msTimer_module = 0;
	}

	return;
}

void dispButtonPress( void )
{
	static int backlightOutTimeSecond;
	static int backlightOutTimeMinute;

	static int menuTimeoutSecond = -1;
	static int menuTimeoutMinute = -1;

	static bool firstRun = true;

	if( firstRun == true )
	{
		firstRun = false;

		backlightOutTimeSecond = timeSecond_global;
		backlightOutTimeMinute = ( timeMinute_global + BACKLIGHT_TIMEOUT_MINUTES ) % 60;
	}


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
			backlightOutTimeSecond = timeSecond_global;
			backlightOutTimeMinute = ( timeMinute_global + BACKLIGHT_TIMEOUT_MINUTES ) % 60;
			menuTimeoutSecond = timeSecond_global + MENU_TIMEOUT_SECONDS;
			menuTimeoutMinute = timeMinute_global;
		}

		// _RB4 is pin 9
		if( !BTN_2 )
			button2Flag_global = 0;
		else if( !button2Flag_global )
		{// && _RB8 == 1
			button2Flag_global = 1;
			backlightOutTimeSecond = timeSecond_global;
			backlightOutTimeMinute = ( timeMinute_global + BACKLIGHT_TIMEOUT_MINUTES ) % 60;
			menuTimeoutSecond = timeSecond_global + MENU_TIMEOUT_SECONDS;
			menuTimeoutMinute = timeMinute_global;
		}

		// _RA3 is pin 8
		if( !BTN_1 )
			button1Flag_global = 0;
		else if( !button1Flag_global )
		{// && _RA4 == 1
			button1Flag_global = 1;
			backlightOutTimeSecond = timeSecond_global;
			backlightOutTimeMinute = ( timeMinute_global + BACKLIGHT_TIMEOUT_MINUTES ) % 60;
			menuTimeoutSecond = timeSecond_global + MENU_TIMEOUT_SECONDS;
			menuTimeoutMinute = timeMinute_global;
		}

		// _RA2 is pin 7
		if( !BTN_0 )
			button0Flag_global = 0;
		else if( !button0Flag_global )
		{// && _RB4 == 1
			button0Flag_global = 1;
			backlightOutTimeSecond = timeSecond_global;
			backlightOutTimeMinute = ( timeMinute_global + BACKLIGHT_TIMEOUT_MINUTES ) % 60;
			menuTimeoutSecond = timeSecond_global + MENU_TIMEOUT_SECONDS;
			menuTimeoutMinute = timeMinute_global;
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
		backlightOutTimeSecond = timeSecond_global;
		backlightOutTimeMinute = ( timeMinute_global + BACKLIGHT_TIMEOUT_MINUTES ) % 60;
		menuTimeoutSecond = timeSecond_global + MENU_TIMEOUT_SECONDS;
		menuTimeoutMinute = timeMinute_global;
	}


	// make sure the timeout seconds and minutes
	// if the timeout is 3600 or more seconds you need your head checked
	while( menuTimeoutSecond >= 60 )
	{
		menuTimeoutSecond -= 60;
		menuTimeoutMinute += 1;
		if( menuTimeoutMinute >= 60 )
		{
			menuTimeoutMinute = 0;
		}
	}

	backlightOutTimeSecond = 0;

	if( ( timeMinute_global == menuTimeoutMinute ) && ( timeSecond_global == menuTimeoutSecond ) )
	{

		if(
		 ( ( menuState_global != MENU_ALARM ) || ( menuState_global == MENU_ALARM ) )
		 && ( menuState_global != MENU_DEBUG )
		 && ( isBooting_global == false )
		 )
		{

			// valid 'stationary' menus are BASIC and DETAIL
			// parameters are still collected when on these menus
			if( menuState_global != MENU_HOME_DETAIL )
			{
				menuState_global = MENU_HOME_BASIC;
			}
		}
	}

	if(
	 ( timeMinute_global == backlightOutTimeMinute ) && ( timeSecond_global == backlightOutTimeSecond )
	 && ( ( menuState_global != MENU_ALARM ) || ( menuState_global == MENU_ALARM ) )
	 && ( menuState_global != MENU_DEBUG )
	 && ( isBooting_global == false )
	 )
	{
		menuState_global = MENU_HOME_BASIC;
		if( BACKLIGHT_NORMAL == true )
		{

			BACKLIGHT = 0; // turn off backlight
		}
	}

	return;
}

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
		__delay_ms( timeOn );
		BACKLIGHT = 0;
	}

	return;
}