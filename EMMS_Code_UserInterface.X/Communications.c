/****************
 INCLUDES
 only include the header files that are required
 ****************/
#include <stdlib.h>

#include "common.h"
#include "Communications.h"
#include "DisplayMain.h"
#include "DisplayMenu.h"
#include "DisplayRTCC.h"

/****************
 MACROS
 ****************/
#define BAUD_UART2 19200

#define BUFFER_LENGTH 250

#define BUF_SIZE_CHAR 5
#define BUF_SIZE_INT 7
#define BUF_SIZE_LONG 12


#define PARAMETER_MAX_COUNT 7
#define PARAMETER_MAX_LENGTH 20

//#define CHAR_NULL '\0'
#define COMMAND_SEND_RECEIVE_PRIMER_CHAR '#' // something to run the SPI clock so data can be received
#define COMMAND_START_CHAR '!'
#define COMMAND_END_CHAR '*'
#define COMMAND_DELIMETER ';'
#define COMMAND_XSUM_CHAR '$'




#define UART_LARGE_BUFFER_SIZE 10

/****************
 VARIABLES
 these are the globals required by only this c file
 there should be as few of these as possible to help keep things clean
 variables required by other c functions should be here and also in the header .h file
 as external
 ****************/
// external
// internal only

enum receive_status
{
	receive_waiting,
	receive_in_command,
	receive_end_command
};

struct buffer_struct
{
	char data_buffer[ BUFFER_LENGTH ];
	unsigned char write_position;
	unsigned char read_position;
};

volatile unsigned char uartBufferLarge_module[ UART_LARGE_BUFFER_SIZE];
volatile unsigned char uartBufferLargeCount_module = 0;


/****************
 FUNCTION PROTOTYPES
 only include functions called from within this code
 external functions should be in the header
 ideally these are in the same order as in the code listing
 any functions used internally and externally (prototype here and in the .h file)
	 should be marked
 *****************/

// internal

bool checkOnOff( char *toCheck );
void fillOnOff( char *buf, bool checkValue );
void zeroPad_itoa( char *output, int num, int minDigits );

void setModuleInfoThis( struct buffer_struct *send_buffer, int moduleInfoIndex );
void readModuleInfo( struct buffer_struct *send_buffer, unsigned char moduleNumber, unsigned char moduleInfoNumber );

void readRemoteMeterName( struct buffer_struct *send_buffer );

void readRemoteTime( struct buffer_struct *send_buffer );
void setRemoteTime( struct buffer_struct *send_buffer );

void readRemoteAlarm( struct buffer_struct *send_buffer );
void setRemoteAlarm( struct buffer_struct *send_buffer );


void readRemotePassword( struct buffer_struct *send_buffer );
void setRemotePassword( struct buffer_struct *send_buffer );

void readRemoteEmergency( struct buffer_struct *send_buffer );
void setRemoteEmergency( struct buffer_struct *send_buffer );

void readRemoteResetTime( struct buffer_struct *send_buffer );
void setRemoteResetTime( struct buffer_struct *send_buffer );

void readRemoteRelay( struct buffer_struct *send_buffer );
void setRemoteRelay( struct buffer_struct *send_buffer );

void readRemoteStat( struct buffer_struct *send_buffer );

void readRemoteCBver( struct buffer_struct *send_buffer );

void readRemotePowerFailTimes( struct buffer_struct *send_buffer );

void readRemotePowerData( struct buffer_struct *send_buffer );

struct buffer_struct * command_builder_external_helper( bool init, struct buffer_struct *send_buffer );

// not yet sorted

bool UART_receive_data_char( char *data );

void set_current_port( unsigned char * );
enum receive_status receive_data( struct buffer_struct * );
bool xSumCheck( char* check_buffer );
bool process_data( struct buffer_struct *receive_buffer, struct buffer_struct *send_buffer );
void process_data_parameterize( char parameters[PARAMETER_MAX_COUNT][PARAMETER_MAX_LENGTH], struct buffer_struct *buffer_to_parameterize );
bool process_data_parameters( char parameters[PARAMETER_MAX_COUNT][PARAMETER_MAX_LENGTH], struct buffer_struct *send_buffer );

void command_builder1( struct buffer_struct *send_buffer, char* data1 );
void command_builder2( struct buffer_struct *send_buffer, char* data1, char* data2 );
void command_builder3( struct buffer_struct *send_buffer, char* data1, char* data2, char* data3 );
void command_builder4( struct buffer_struct *send_buffer, char* data1, char* data2, char* data3, char* data4 );
void command_builder5( struct buffer_struct *send_buffer, char* data1, char* data2, char* data3, char* data4, char* data5 );
void command_builder6( struct buffer_struct *send_buffer, char* data1, char* data2, char* data3, char* data4, char* data5, char* data6 );
void command_builder7( struct buffer_struct *send_buffer, char* data1, char* data2, char* data3, char* data4, char* data5, char* data6, char* data7 );
char command_builder_add_char( struct buffer_struct *send_buffer, char data );
int command_builder_add_string( struct buffer_struct *send_buffer, char *data );
void xsum_builder( struct buffer_struct *send_buffer, int xsum );

bool send_data( struct buffer_struct *send_buffer );
bool UART_send_data_char( char data );

bool strmatch( char* a, char* b );
int strcmp2( char* a, char* b );
void strcpy2( char* dest, char* source );

void initUART( void );

void send_end_of_transmission( struct buffer_struct *send_buffer );
void com_command_testLED( struct buffer_struct * send_buffer );
void com_command_setPower( struct buffer_struct * send_buffer );
void com_command_setEnergyUsed( struct buffer_struct * send_buffer );
void com_command_setVolts( struct buffer_struct * send_buffer );
void com_command_setAmps( struct buffer_struct * send_buffer );
void com_command_readCalibration( struct buffer_struct * send_buffer );
void com_command_setVersion( struct buffer_struct * send_buffer );


void com_command_readRemoteEnergyAllocation( void );
void readRemoteEnergyAllocation( struct buffer_struct *send_buffer );
void com_command_setRemoteEnergyAllocation( void );
void setRemoteEnergyAllocation( struct buffer_struct *send_buffer );
void com_command_setRemoteAllocationAdd( void );
void setRemoteAllocationAdd( struct buffer_struct *send_buffer );

/****************
 CODE
 ****************/
void communications( bool firstTime )
{
	static struct buffer_struct receive_buffer;

	static struct buffer_struct send_buffer;

	static bool end_of_transmission_received = false;
	bool no_more_to_send; // here to make this more readable

	static enum receive_status receive_current_state;

	if( firstTime == true )
	{
		initUART( );

		send_buffer.read_position = 0;
		send_buffer.write_position = 0;

		command_builder_external_helper( true, &send_buffer );
	}
	else
	{
		receive_current_state = receive_data( &receive_buffer );

		switch( receive_current_state )
		{
			case receive_waiting:
				// don't need to worry about it too much

				break;
			case receive_in_command:
				// don't need to worry about it too much


				break;
			case receive_end_command:

				if( xSumCheck( receive_buffer.data_buffer ) == true )
				{
					if( process_data( &receive_buffer, &send_buffer ) == true )
					{
						end_of_transmission_received = true;
					}
				}
				break;
		}

		no_more_to_send = send_data( &send_buffer );

		// dont need this - port is never closed

		//	static bool last_state_active = false;
		//	if( PORTBbits.SS2 == 0b1 )
		//	{
		//	    last_state_active = false;
		//	}
		//	else
		//	{
		//	    if( last_state_active == false )
		//	    {
		//		resetCommunications( &send_buffer );
		//	    }
		//
		//	    last_state_active = true;
		//	}

	}

	return;
}



// Will check if the Checksum in a message is accurate to ensure accuracy

bool xSumCheck( char* checkBuffer )
{
	// calculate the xsum for the received command
	//		ignore the starting '!' and everything including and after the xsum delimeter '$'
	// grab the xsum characters after the xsum delimeter
	//		convert to int
	// compare and return bool result

	int receiveBufferPos;
	int xSumAdderValue;
	bool xSumMatches;

	receiveBufferPos = 1; // start at one to skip the start '!' character in position 0
	xSumAdderValue = 0;

	while(
		 ( receiveBufferPos < BUFFER_LENGTH ) // this check first - if it fails the other checks are not done - this makes sure the receiveBufferPos is never out of bounds
		 && ( checkBuffer[ receiveBufferPos ] != COMMAND_XSUM_CHAR )
		 && ( checkBuffer[ receiveBufferPos ] != COMMAND_END_CHAR )
		 )
	{
		xSumAdderValue += checkBuffer[ receiveBufferPos ];
		receiveBufferPos++;
	}

	if( checkBuffer[ receiveBufferPos ] == COMMAND_XSUM_CHAR )
	{
		char xSumChars [BUF_SIZE_INT]; // size must be maximum size of int data type + null character
		int xSumCharsPos;
		int xSumCharsValue;

		receiveBufferPos++; // add one to skip over the COMMAND_XSUM_CHAR 
		xSumCharsPos = 0;
		xSumChars[xSumCharsPos] = CHAR_NULL; // make sure there is always a null termination so the atoi does not freak out

		while(
			 ( receiveBufferPos < BUFFER_LENGTH ) // this check first - if it fails the other checks are not done - this makes sure the receiveBufferPos is never out of bounds
			 && ( checkBuffer[ receiveBufferPos ] != COMMAND_END_CHAR )
			 )
		{
			xSumChars[xSumCharsPos] = checkBuffer[ receiveBufferPos ];
			xSumCharsPos++;
			if( xSumCharsPos >= BUF_SIZE_INT )
			{
				xSumCharsPos = ( BUF_SIZE_INT - 1 );
			}
			xSumChars[xSumCharsPos] = CHAR_NULL;
			receiveBufferPos++;
		}

		xSumCharsValue = atoi( xSumChars );

		xSumMatches = ( xSumAdderValue == xSumCharsValue );
	}
	else
	{
		xSumMatches = false; // if we set to true here we can handle commands without a xsum
		// but this could also allow mangled commands to come through
	}

	return xSumMatches;
}

enum receive_status receive_data( struct buffer_struct * receive_buffer )
{
	char data;
	bool somethingReceived;

	static enum receive_status my_status = receive_waiting;

	if( my_status == receive_end_command )
	{
		my_status = receive_waiting;
	}


	somethingReceived = UART_receive_data_char( &data );

	while( ( somethingReceived == true ) && ( my_status != receive_end_command ) )
	{
		if( ( data == COMMAND_START_CHAR ) && ( my_status != receive_in_command ) )
		{
			my_status = receive_in_command;
			receive_buffer->read_position = 0;
			receive_buffer->write_position = 0;
		}

		if( my_status == receive_in_command )
		{
			receive_buffer->data_buffer[ receive_buffer->write_position] = data;

			receive_buffer->write_position++;
			if( receive_buffer->write_position >= BUFFER_LENGTH )
			{
				receive_buffer->write_position = ( BUFFER_LENGTH - 1 );
			}
		}

		if( ( my_status == receive_in_command ) && ( data == COMMAND_END_CHAR ) )
		{
			my_status = receive_end_command;
		}
		somethingReceived = UART_receive_data_char( &data );

	}

	return my_status;
}

bool process_data( struct buffer_struct *receive_buffer, struct buffer_struct * send_buffer )
{
	bool end_of_transmission_received;

	// if we are here then the receive buffer must have good data with start and end command characters
	// the characters are not included as they were stripped from the incoming data

	char parameters[PARAMETER_MAX_COUNT][PARAMETER_MAX_LENGTH];

	process_data_parameterize( parameters, receive_buffer );

	end_of_transmission_received = process_data_parameters( parameters, send_buffer );

	return end_of_transmission_received;
}

void process_data_parameterize( char parameters[PARAMETER_MAX_COUNT][PARAMETER_MAX_LENGTH], struct buffer_struct * buffer_to_parameterize )
{
	unsigned char parameter_position = 0;
	unsigned char parameter_index = 0;

	// only one command is expected due to the way we read
	// go through buffer until we hit end char or end of buffer

	// this is super important - we must initialize the entire array
	// if we do not we risk passing junk into some functions that assume strings and check for NULL
	// without NULL a string function could run forever until we die from old age
	// even then it would keep running
	for( int inx = 0; inx < PARAMETER_MAX_COUNT; inx++ )
	{
		parameters[inx][0] = CHAR_NULL;
	}

	while(
		 ( buffer_to_parameterize->read_position < BUFFER_LENGTH ) // this check first - if it fails the other checks are not done - this makes sure the receiveBufferPos is never out of bounds
		 &&( buffer_to_parameterize->data_buffer[buffer_to_parameterize->read_position ] != COMMAND_END_CHAR )
		 && ( buffer_to_parameterize->read_position != buffer_to_parameterize->write_position )
		 && ( buffer_to_parameterize->data_buffer[buffer_to_parameterize->read_position ] != COMMAND_XSUM_CHAR )
		 )
	{
		switch( buffer_to_parameterize->data_buffer[buffer_to_parameterize->read_position] )
		{
			case COMMAND_START_CHAR:
				// this character should never appear
				break;
			case COMMAND_DELIMETER:
				// move to next parameter
				parameter_position = 0;
				parameter_index++;

				if( parameter_index >= PARAMETER_MAX_COUNT )
				{
					// if we run out of parameters just overwrite the last one
					// we should never have this case, but this keeps us from crashing and burning
					parameter_index = ( PARAMETER_MAX_COUNT - 1 );
				}

				break;
			default:
				// add the character to the current parameter
				parameters[parameter_index][parameter_position] = buffer_to_parameterize->data_buffer[buffer_to_parameterize->read_position];
				parameter_position++;
				if( parameter_position >= PARAMETER_MAX_LENGTH )
				{
					// if our parameter is too long, just overwrite the last character
					// we should never have this case, but this keeps us from crashing and burning
					parameter_position = ( PARAMETER_MAX_LENGTH - 1 );
				}

				// always make the last character a null
				parameters[parameter_index][parameter_position] = CHAR_NULL;
				break;
		}

		buffer_to_parameterize->read_position++;
	}

	return;
}

bool process_data_parameters( char parameters[PARAMETER_MAX_COUNT][PARAMETER_MAX_LENGTH], struct buffer_struct * send_buffer )
{
	bool end_of_transmission_received = false;


	if( strmatch( parameters[0], "END" ) == true )
	{

		end_of_transmission_received = true;
	}
	else if( strmatch( parameters[0], "Set" ) == true )
	{
		if( strmatch( parameters[1], "Time" ) == true )
		{
			//	    //DEBUG TOM
			//	    writeToDisplay( parameters[0], 0, 40 );
			//	    writeToDisplay( parameters[1], 20, 40 );
			//	    writeToDisplay( parameters[2], 40, 40 );
			//	    writeToDisplay( parameters[3], 60, 40 );
			//	    delayMS( 10000 );

			char timeDayBuf[3];
			char timeMonthBuf[3];
			char timeYearBuf[3];

			char timeHourBuf[3];
			char timeMinuteBuf[3];
			char timeSecondBuf[3];

			// DD-MM-YY
			timeDayBuf[0] = parameters[2][0];
			timeDayBuf[1] = parameters[2][1];
			timeDayBuf[2] = CHAR_NULL;

			timeMonthBuf[0] = parameters[2][3];
			timeMonthBuf[1] = parameters[2][4];
			timeMonthBuf[2] = CHAR_NULL;

			timeYearBuf[0] = parameters[2][6];
			timeYearBuf[1] = parameters[2][7];
			timeYearBuf[2] = CHAR_NULL;

			// HH:MM:SS
			timeHourBuf[0] = parameters[3][0];
			timeHourBuf[1] = parameters[3][1];
			timeHourBuf[2] = CHAR_NULL;

			timeMinuteBuf[0] = parameters[3][3];
			timeMinuteBuf[1] = parameters[3][4];
			timeMinuteBuf[2] = CHAR_NULL;

			timeSecondBuf[0] = parameters[3][6];
			timeSecondBuf[1] = parameters[3][7];
			timeSecondBuf[2] = CHAR_NULL;


			char newTimeDay;
			char newTimeMonth;
			char newTimeYear;

			char newTimeHour;
			char newTimeMinute;
			char newTimeSecond;

			newTimeDay = atoi( timeDayBuf );
			newTimeMonth = atoi( timeMonthBuf );
			newTimeYear = atoi( timeYearBuf );

			newTimeHour = atoi( timeHourBuf );
			newTimeMinute = atoi( timeMinuteBuf );
			newTimeSecond = atoi( timeSecondBuf );

			writeTime( newTimeYear, newTimeMonth, newTimeDay, newTimeHour, newTimeMinute, newTimeSecond );

			rtccReadTime( );

			command_builder2( send_buffer, "Conf", "Time" );

		}
		else if( strmatch( parameters[1], "EnAl" ) == true )
		{
			energyAllocated_global = atol( parameters[2] );

			command_builder2( send_buffer, "Conf", "EnAl" );

		}
		else if( strmatch( parameters[1], "Alarm" ) == true )
		{
			//2	    audibleAlarmBuf On Off;
			//3	    alarmOnePercentThresholdBuf INT;
			//4	    alarmTwoPercentThresholdBuf INT;

			audibleAlarm_global = checkOnOff( parameters[2] );

			alarm1PercentThreshold_global = atoi( parameters[3] );
			alarm2PercentThreshold_global = atoi( parameters[4] );

			command_builder2( send_buffer, "Conf", "Alarm" );

		}
		else if( strmatch( parameters[1], "Pass" ) == true )
		{
			passwordSet_global[0] = parameters[2][0];
			passwordSet_global[1] = parameters[2][1];
			passwordSet_global[2] = parameters[2][2];
			passwordSet_global[3] = parameters[2][3];
			passwordSet_global[4] = parameters[2][4];
			passwordSet_global[5] = parameters[2][5];

			command_builder2( send_buffer, "Conf", "Pass" );

		}
		else if( strmatch( parameters[1], "Emer" ) == true )
		{
			emerButtonEnable_global = checkOnOff( parameters[2] );

			emerButtonEnergyAllocate_global = atoi( parameters[3] );

			command_builder2( send_buffer, "Conf", "Emer" );
		}
		else if( strmatch( parameters[1], "RstTim" ) == true )
		{

			resetTimeHour_global = atoi( parameters[2] );
			resetTimeMinute_global = atoi( parameters[3] );

			tempResetHour_global = resetTimeHour_global;
			tempResetMinute_global = resetTimeMinute_global;

			command_builder2( send_buffer, "Conf", "RstTim" );

		}
		else if( strmatch( parameters[1], "Relay" ) == true )
		{

			// relay can have 3 modes
			//  0   =   off
			//  1   =   on
			//  2   =   auto

			if( strmatch( parameters[2], "Off" ) == true )
			{
				relayMode_global = 0;
			}
			else if( strmatch( parameters[2], "On" ) == true )
			{
				relayMode_global = 1;
			}
			else if( strmatch( parameters[2], "Auto" ) == true )
			{
				relayMode_global = 2;
			}

			command_builder2( send_buffer, "Conf", "Relay" );

		}
		else if( strmatch( parameters[1], "Stat" ) == true )
		{

			totalUsed_global = atol( parameters[2] );
			previousDayUsed_global = atol( parameters[2] );

			command_builder2( send_buffer, "Conf", "Stat" );

		}
		else if( strmatch( parameters[1], "MName" ) == true )
		{

			strcpy2( meterNameString_global, parameters[2] );

			command_builder2( send_buffer, "Conf", "MName" );
		}
		else if( strmatch( parameters[1], "PwrFail" ) == true )
		{
			powerDownTime_global[0] = parameters[2][0];
			powerDownTime_global[1] = parameters[2][1];
			powerDownTime_global[2] = parameters[2][2];
			powerDownTime_global[3] = parameters[2][3];
			powerDownTime_global[4] = parameters[2][4];
			powerDownTime_global[5] = parameters[2][5];
			powerDownTime_global[6] = parameters[2][6];
			powerDownTime_global[7] = parameters[2][7];
			powerDownTime_global[8] = parameters[2][8];
			powerDownTime_global[9] = parameters[2][9];
			powerDownTime_global[10] = parameters[2][10];
			powerDownTime_global[11] = CHAR_NULL;

			powerUpTime_global[0] = parameters[3][0];
			powerUpTime_global[1] = parameters[3][1];
			powerUpTime_global[2] = parameters[3][2];
			powerUpTime_global[3] = parameters[3][3];
			powerUpTime_global[4] = parameters[3][4];
			powerUpTime_global[5] = parameters[3][5];
			powerUpTime_global[6] = parameters[3][6];
			powerUpTime_global[7] = parameters[3][7];
			powerUpTime_global[8] = parameters[3][8];
			powerUpTime_global[9] = parameters[3][9];
			powerUpTime_global[10] = parameters[3][10];
			powerUpTime_global[11] = CHAR_NULL;

			command_builder2( send_buffer, "Conf", "PwrFail" );

		}
		else if( strmatch( parameters[1], "PwrData" ) == true )
		{
			energyAllocated_global = atol( parameters[2] );
			energyUsed_global = atol( parameters[3] );
			powerLoad_global = atol( parameters[4] );

			command_builder2( send_buffer, "Conf", "PwrData" );

		}

			// the following parameter is too long
			// max length is defined as 10
		else if( strmatch( parameters[1], "Calibrate" ) == true )
		{
			// set the calibration value for the current sense, if required
		}
		else if( strmatch( parameters[1], "EnUsed" ) == true )
		{
			// set the Energy used
			// this likely means that the command board had a stored power used greater than we have here.
			// this happens when the power is lost - current sense starts at 0, command board stores in EEPROM

			//meterEnergyUsed = atol( parameters[2] );
			//	    com_command_setEnergyUsed( send_buffer );
		}
		else if( strmatch( parameters[1], "ModInfo" ) == true )
		{
			// !Set;Mod;X;Y;____$xsum*
			//	parameter 0 = Set
			//	parameter 1 = Mod
			//	parameter 2 = X module number
			//				-1 for CB to make determination
			//				-1 is invalid here and will be ignored if this is not the CB
			//	parameter 3 = Y info number (0 to 4))
			//	parameter 4 = info text

			bool commandGood;
			int moduleNumber;
			int moduleInfoNumber;

			moduleNumber = atoi( parameters[2] );
			moduleInfoNumber = atoi( parameters[3] );

			commandGood = false;

			if( ( moduleNumber >= 0 ) && ( moduleNumber < MODULE_COUNT ) )
			{
				switch( moduleInfoNumber )
				{
					case 0:
						strcpy2( moduleInfo_global[moduleNumber].info0, parameters[4] );
						commandGood = true;
						break;
					case 1:
						strcpy2( moduleInfo_global[moduleNumber].info1, parameters[4] );
						commandGood = true;
						break;
					case 2:
						strcpy2( moduleInfo_global[moduleNumber].info2, parameters[4] );
						commandGood = true;
						break;
					case 3:
						strcpy2( moduleInfo_global[moduleNumber].info3, parameters[4] );
						commandGood = true;
						break;
					case 4:
						strcpy2( moduleInfo_global[moduleNumber].info4, parameters[4] );
						commandGood = true;
						break;
				}
			}

			// only send a confirmation if the module info was actually used
			// otherwise send an error
			if( commandGood == true )
			{
				command_builder2( send_buffer, "Conf", "ModInfo" );
			}
			else
			{
				command_builder2( send_buffer, "Err", "ModInfo" );
			}
		}

	}
	else if( strmatch( parameters[0], "Read" ) == true )
	{
		if( strmatch( parameters[1], "Time" ) == true )
		{
			setRemoteTime( send_buffer );
		}
		if( strmatch( parameters[1], "EnAl" ) == true )
		{
			setRemoteEnergyAllocation( send_buffer );
		}
		if( strmatch( parameters[1], "Alarm" ) == true )
		{
			setRemoteAlarm( send_buffer );
		}
		else if( strmatch( parameters[1], "Pass" ) == true )
		{
			setRemotePassword( send_buffer );
		}
		else if( strmatch( parameters[1], "Emer" ) == true )
		{
			setRemoteEmergency( send_buffer );
		}
		else if( strmatch( parameters[1], "Relay" ) == true )
		{
			setRemoteRelay( send_buffer );
		}
	}
	else if( strmatch( parameters[0], "Data" ) == true )
	{
		if( strmatch( parameters[1], "LEDB" ) == true )
		{
			if( strmatch( parameters[2], "On" ) == true )
			{
				command_builder3( send_buffer, "Set", "LEDB", "Off" );
			}
			else if( strmatch( parameters[2], "Off" ) == true )
			{
				command_builder3( send_buffer, "Set", "LEDB", "On" );
			}
		}
	}
	else if( strmatch( parameters[0], "Conf" ) == true )
	{
		if( strmatch( parameters[1], "LEDB" ) == true )
		{
			send_end_of_transmission( send_buffer );
		}
		else if( strmatch( parameters[1], "Watts" ) == true )
		{
			send_end_of_transmission( send_buffer );
		}
		else if( strmatch( parameters[1], "EnUsed" ) == true )
		{
			send_end_of_transmission( send_buffer );
		}
		else if( strmatch( parameters[1], "Volts" ) == true )
		{
			send_end_of_transmission( send_buffer );
		}
		else if( strmatch( parameters[1], "Amps" ) == true )
		{
			send_end_of_transmission( send_buffer );
		}
	}

	// add new parameters as necessary
	// NEVER check for a higher parameter number than we allocated for.
	// see earlier comments about NULLS and dying from old age

	return end_of_transmission_received;
}

void command_builder1( struct buffer_struct *send_buffer, char* data1 )
{
	command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
	command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
	command_builder_add_char( send_buffer, COMMAND_START_CHAR );

	int xsum = 0;
	xsum += command_builder_add_string( send_buffer, data1 );

	xsum_builder( send_buffer, xsum );

	return;
}

void command_builder2( struct buffer_struct *send_buffer, char* data1, char* data2 )
{
	command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
	command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
	command_builder_add_char( send_buffer, COMMAND_START_CHAR );

	int xsum = 0;
	xsum += command_builder_add_string( send_buffer, data1 );
	xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
	xsum += command_builder_add_string( send_buffer, data2 );

	xsum_builder( send_buffer, xsum );

	return;
}

void command_builder3( struct buffer_struct *send_buffer, char* data1, char* data2, char* data3 )
{
	command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
	command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
	command_builder_add_char( send_buffer, COMMAND_START_CHAR );

	int xsum = 0;
	xsum += command_builder_add_string( send_buffer, data1 );
	xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
	xsum += command_builder_add_string( send_buffer, data2 );
	xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
	xsum += command_builder_add_string( send_buffer, data3 );

	xsum_builder( send_buffer, xsum );

	return;
}

void command_builder4( struct buffer_struct *send_buffer, char* data1, char* data2, char* data3, char* data4 )
{
	command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
	command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
	command_builder_add_char( send_buffer, COMMAND_START_CHAR );

	int xsum = 0;
	xsum += command_builder_add_string( send_buffer, data1 );
	xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
	xsum += command_builder_add_string( send_buffer, data2 );
	xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
	xsum += command_builder_add_string( send_buffer, data3 );
	xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
	xsum += command_builder_add_string( send_buffer, data4 );

	xsum_builder( send_buffer, xsum );

	return;
}

void command_builder5( struct buffer_struct *send_buffer, char* data1, char* data2, char* data3, char* data4, char* data5 )
{
	command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
	command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
	command_builder_add_char( send_buffer, COMMAND_START_CHAR );

	int xsum = 0;
	xsum += command_builder_add_string( send_buffer, data1 );
	xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
	xsum += command_builder_add_string( send_buffer, data2 );
	xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
	xsum += command_builder_add_string( send_buffer, data3 );
	xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
	xsum += command_builder_add_string( send_buffer, data4 );
	xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
	xsum += command_builder_add_string( send_buffer, data5 );

	xsum_builder( send_buffer, xsum );

	return;
}

void command_builder6( struct buffer_struct *send_buffer, char* data1, char* data2, char* data3, char* data4, char* data5, char* data6 )
{
	command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
	command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
	command_builder_add_char( send_buffer, COMMAND_START_CHAR );

	int xsum = 0;
	xsum += command_builder_add_string( send_buffer, data1 );
	xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
	xsum += command_builder_add_string( send_buffer, data2 );
	xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
	xsum += command_builder_add_string( send_buffer, data3 );
	xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
	xsum += command_builder_add_string( send_buffer, data4 );
	xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
	xsum += command_builder_add_string( send_buffer, data5 );
	xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
	xsum += command_builder_add_string( send_buffer, data6 );

	xsum_builder( send_buffer, xsum );

	return;
}

void command_builder7( struct buffer_struct *send_buffer, char* data1, char* data2, char* data3, char* data4, char* data5, char* data6, char* data7 )
{
	command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
	command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
	command_builder_add_char( send_buffer, COMMAND_START_CHAR );

	int xsum = 0;
	xsum += command_builder_add_string( send_buffer, data1 );
	xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
	xsum += command_builder_add_string( send_buffer, data2 );
	xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
	xsum += command_builder_add_string( send_buffer, data3 );
	xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
	xsum += command_builder_add_string( send_buffer, data4 );
	xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
	xsum += command_builder_add_string( send_buffer, data5 );
	xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
	xsum += command_builder_add_string( send_buffer, data6 );
	xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
	xsum += command_builder_add_string( send_buffer, data7 );

	xsum_builder( send_buffer, xsum );

	return;
}

void xsum_builder( struct buffer_struct *send_buffer, int xsum )
{

	//command_builder_add_char( send_buffer, COMMAND_DELIMETER ); // REMOVE THIS ONCE XSUM CHECK IS IMPLEMENTED
	command_builder_add_char( send_buffer, COMMAND_XSUM_CHAR ); //@
	char xsumBuf[16]; //allocate space for XSUM
	itoa( xsumBuf, xsum, 10 ); //convert XSUM from int into to string.
	command_builder_add_string( send_buffer, xsumBuf ); //add XSUM to send buffer 

	command_builder_add_char( send_buffer, COMMAND_END_CHAR );

	return;
}

char command_builder_add_char( struct buffer_struct *send_buffer, char data )
{
	send_buffer->data_buffer[send_buffer->write_position] = data;

	send_buffer->write_position++;
	if( send_buffer->write_position >= BUFFER_LENGTH )
	{
		send_buffer->write_position = 0;
	}

	return data; // return the character for XSum calculation
}

int command_builder_add_string( struct buffer_struct *send_buffer, char *data_string )
{
	int xsum = 0;
	for( int inx = 0; data_string[inx] != CHAR_NULL; inx++ )
	{
		xsum += command_builder_add_char( send_buffer, data_string[inx] );
	}

	return xsum;
}

struct buffer_struct * command_builder_external_helper( bool init, struct buffer_struct *send_buffer )
{

	static struct buffer_struct *send_buffer_keep;

	if( init == true )
	{
		send_buffer_keep = send_buffer;
	}


	return send_buffer_keep;
}

bool send_data( struct buffer_struct * send_buffer )
{
	bool send_end;

	if( send_buffer->read_position == send_buffer->write_position )
	{
		send_end = true;
		UART_send_data_char( '\0' );
		send_buffer->read_position = 0;
		send_buffer->write_position = 0;
	}
	else
	{
		send_end = false;

		char data;

		data = send_buffer->data_buffer[send_buffer->read_position];

		if( UART_send_data_char( data ) == true )
		{
			send_buffer->read_position++;
			if( send_buffer->read_position >= BUFFER_LENGTH )
			{
				send_buffer->read_position = 0;
			}
		}
	}

	return send_end;
}

bool strmatch( char* a, char* b )
{
	int result;
	bool match;

	result = strcmp2( a, b );

	match = ( result == 0 ) ? true : false;

	return match;
}

int strcmp2( char* a, char* b )
{
	int inx = 0;
	int match = 0;

	while( ( a[inx] != CHAR_NULL ) && ( b[inx] != CHAR_NULL ) && ( match == 0 ) )
	{
		if( a[inx] > b[inx] )
		{
			match = 1;
		}
		else if( a[inx] < b[inx] )
		{
			match = -1;
		}
		else if( a[inx] == b[inx] )
		{
			//do nothing = never reset to zero
		}

		inx++;
	}


	if( ( a[inx] == CHAR_NULL ) && ( b[inx] != CHAR_NULL ) )
	{
		match = -1;
	}
	else if( ( a[inx] != CHAR_NULL ) && ( b[inx] == CHAR_NULL ) )
	{
		match = 1;
	}

	return match;
}

void strcpy2( char* dest, char* source )
{
	// make sure there is a null termination
	// make sure not to run out of bounds - there is no checking here

	while( *source != CHAR_NULL )
	{
		*dest = *source;
		dest++;
		source++;
	}
	*dest = CHAR_NULL;

	return;
}

bool checkOnOff( char *toCheck )
{
	bool isOn = false;

	if( strmatch( toCheck, "On" ) == true )
	{
		isOn = true;
	}
	else if( strmatch( toCheck, "Off" ) == true )
	{
		isOn = false;
	}

	return isOn;
}

void fillOnOff( char *buf, bool checkValue )
{
	if( checkValue == false )
	{
		buf[0] = 'O';
		buf[1] = 'f';
		buf[2] = 'f';
		buf[3] = CHAR_NULL;

	}
	else
	{
		buf[0] = 'O';
		buf[1] = 'n';
		buf[2] = CHAR_NULL;
		buf[3] = CHAR_NULL;
	}

	return;
}

void zeroPad_itoa( char *output, int num, int minDigits )
{
	char temp[BUF_SIZE_INT];

	int rawLen;

	itoa( temp, num, 10 );

	rawLen = 0;
	while( temp[rawLen] != CHAR_NULL )
	{
		rawLen++;
	}

	// rawLen now contains the length of the converted number

	int padding;

	padding = minDigits - rawLen;

	int inxOutput;
	for( inxOutput = 0; inxOutput < padding; inxOutput++ )
	{
		output[inxOutput] = '0';
	}

	int inxTemp;
	inxTemp = 0;
	while( temp[inxTemp] != CHAR_NULL )
	{
		output[inxOutput] = temp[inxTemp];
		inxOutput++;
		inxTemp++;
	}
	output[inxOutput ] = CHAR_NULL;


	return;
}

bool UART_receive_data_char( char *data )
{
	// need to use interrupts because we have some blocking code to update the display

	bool recvGood = false;

	if( uartBufferLargeCount_module > 0 )
	{
		*data = uartBufferLarge_module[0];
		recvGood = true;

		uartBufferLargeCount_module--;

		for( int inx = 0; inx < uartBufferLargeCount_module; inx++ )
		{
			uartBufferLarge_module[ inx] = uartBufferLarge_module[ inx + 1];
		}


	}

	return recvGood;
}

bool UART_send_data_char( char data )
{
	bool sendGood = false;

	//	if( U2STAbits.UTXBF == 0 )
	// The UART TX Buffer has issues
	// PIC24 Errata (PIC document 80000522) states
	//		filling the TX buffer full with 4 characters causes problems
	//			- workaround - only go to 3 characters, not all 4
	//		UTXBF flag does not work right
	//			- workaround - use buffer empty flag (TRMT))
	// our solution - put one character at a time in the transmit buffer
	//	check to see if the buffer is empty ( TRMT == 1 ) before loading another character
	if( U2STAbits.TRMT == 1 )
	{
		U2TXREG = data;
		sendGood = true;
	}

	return sendGood;
}

void send_end_of_transmission( struct buffer_struct *send_buffer )
{
	command_builder1( send_buffer, "END" );

	return;
}

bool commBufferEmpty( void )
{
	bool bufferEmpty;

	struct buffer_struct *send_buffer;

	send_buffer = command_builder_external_helper( false, NULL );

	if( send_buffer->read_position == send_buffer->write_position )
	{
		bufferEmpty = true;
	}
	else
	{
		bufferEmpty = false;
	}

	return bufferEmpty;
}

void com_command_sendModuleInfoThis( void )
{
	struct buffer_struct *send_buffer;
	static int moduleInfoIndex = 0;

	send_buffer = command_builder_external_helper( false, NULL );

	setModuleInfoThis( send_buffer, moduleInfoIndex );
	setModuleInfoThis( send_buffer, moduleInfoIndex );

	moduleInfoIndex++;
	if( moduleInfoIndex >= MODULE_INFO_COUNT )
	{
		moduleInfoIndex = 0;
	}

	return;
}

void setModuleInfoThis( struct buffer_struct *send_buffer, int moduleInfoIndex )
{
	char moduleInfoIndexBuf[BUF_SIZE_INT];

	itoa( moduleInfoIndexBuf, moduleInfoIndex, 10 );
	//	Set;Mod;0;index;____$*

	switch( moduleInfoIndex )
	{
		case 0:
			command_builder5( send_buffer, "Set", "ModInfo", "-1", moduleInfoIndexBuf, MODULE_INFO_THIS_0 );
			break;
		case 1:
			command_builder5( send_buffer, "Set", "ModInfo", "-1", moduleInfoIndexBuf, MODULE_INFO_THIS_1 );
			break;
		case 2:
			command_builder5( send_buffer, "Set", "ModInfo", "-1", moduleInfoIndexBuf, MODULE_INFO_THIS_2 );
			break;
		case 3:
			command_builder5( send_buffer, "Set", "ModInfo", "-1", moduleInfoIndexBuf, MODULE_INFO_THIS_3 );
			break;
		case 4:
			command_builder5( send_buffer, "Set", "ModInfo", "-1", moduleInfoIndexBuf, MODULE_INFO_THIS_4 );
			break;
	}

	return;
}

void com_command_readModuleInfo( void )
{
	struct buffer_struct *send_buffer;
	static unsigned char moduleNumber = 0;
	static unsigned char moduleInfoNumber = 0;

	send_buffer = command_builder_external_helper( false, NULL );

	readModuleInfo( send_buffer, moduleNumber, moduleInfoNumber );

	moduleInfoNumber++;
	if( moduleInfoNumber >= MODULE_INFO_COUNT )
	{
		moduleInfoNumber = 0;
		moduleNumber++;
		if( moduleNumber >= MODULE_COUNT )
		{
			moduleNumber = 0;
		}
	}

	return;
}

void readModuleInfo( struct buffer_struct *send_buffer, unsigned char moduleNumber, unsigned char moduleInfoNumber )
{
	char moduleNumberBuf[BUF_SIZE_CHAR];
	char moduleInfoNumberBuf[BUF_SIZE_CHAR];

	itoa( moduleNumberBuf, moduleNumber, 10 );
	itoa( moduleInfoNumberBuf, moduleInfoNumber, 10 );

	command_builder4( send_buffer, "Read", "ModInfo", moduleNumberBuf, moduleInfoNumberBuf );

	return;
}

void com_command_readRemoteMeterName( void )
{
	struct buffer_struct *send_buffer;

	send_buffer = command_builder_external_helper( false, NULL );

	readRemoteMeterName( send_buffer );

	return;
}

void readRemoteMeterName( struct buffer_struct *send_buffer )
{
	command_builder2( send_buffer, "Read", "MName" );

	return;
}


void com_command_readRemoteTime( void )
{
	struct buffer_struct *send_buffer;

	send_buffer = command_builder_external_helper( false, NULL );

	readRemoteTime( send_buffer );

	return;
}

void readRemoteTime( struct buffer_struct *send_buffer )
{
	command_builder2( send_buffer, "Read", "Time" );

	return;
}

void com_command_setRemoteTime( void )
{
	struct buffer_struct *send_buffer;

	send_buffer = command_builder_external_helper( false, NULL );

	setRemoteTime( send_buffer );
	setRemoteTime( send_buffer );

	return;
}

void setRemoteTime( struct buffer_struct *send_buffer )
{

	char timeDateBuf[ 9]; //	"DD-MM-YY"
	char timeTimeBuf[9]; //	"HH:MM:SS"

	char timeDateDDBuf[BUF_SIZE_INT];
	char timeDateMMBuf[BUF_SIZE_INT];
	char timeDateYYBuf[BUF_SIZE_INT];

	char timeTimeHHBuf[BUF_SIZE_INT];
	char timeTimeMMBuf[BUF_SIZE_INT];
	//    char timeTimeSSBuf[BUF_SIZE_INT];

	zeroPad_itoa( timeDateDDBuf, tempDay_global, 2 );
	zeroPad_itoa( timeDateMMBuf, tempMonth_global, 2 );
	zeroPad_itoa( timeDateYYBuf, tempYear_global, 2 );
	zeroPad_itoa( timeTimeHHBuf, tempHour_global, 2 );
	zeroPad_itoa( timeTimeMMBuf, tempMin_global, 2 );
	//    zeroPad_itoa( timeTimeSSBuf, tempSecond, 2 );

	//    zeroPad_itoa( timeDateDDBuf, timeDay, 2 );
	//    zeroPad_itoa( timeDateMMBuf, timeMonth, 2 );
	//    zeroPad_itoa( timeDateYYBuf, timeYear, 2 );
	//    zeroPad_itoa( timeTimeHHBuf, timeHour, 2 );
	//    zeroPad_itoa( timeTimeMMBuf, timeMinute, 2 );
	//    zeroPad_itoa( timeTimeSSBuf, timeSecond, 2 );

	timeDateBuf[0] = timeDateDDBuf[0];
	timeDateBuf[1] = timeDateDDBuf[1];
	timeDateBuf[2] = '-';
	timeDateBuf[3] = timeDateMMBuf[0];
	timeDateBuf[4] = timeDateMMBuf[1];
	timeDateBuf[5] = '-';
	timeDateBuf[6] = timeDateYYBuf[0];
	timeDateBuf[7] = timeDateYYBuf[1];
	timeDateBuf[8] = CHAR_NULL;

	timeTimeBuf[0] = timeTimeHHBuf[0];
	timeTimeBuf[1] = timeTimeHHBuf[1];
	timeTimeBuf[2] = ':';
	timeTimeBuf[3] = timeTimeMMBuf[0];
	timeTimeBuf[4] = timeTimeMMBuf[1];
	timeTimeBuf[5] = ':';
	timeTimeBuf[6] = '0'; //  timeTimeSSBuf[0];
	timeTimeBuf[7] = '0'; //timeTimeSSBuf[1];
	timeTimeBuf[8] = CHAR_NULL;


	//    // DEBUG TOM
	//    writeToDisplay( "Set", 0, 20 );
	//    writeToDisplay( "Time", 20, 20 );
	//    writeToDisplay( timeDateBuf, 40, 20 );
	//    writeToDisplay( timeTimeBuf, 60, 20 );
	//    delayMS( 2500 );

	command_builder4( send_buffer, "Set", "Time", timeDateBuf, timeTimeBuf );

	return;
}

void com_command_readRemoteEnergyAllocation( void )
{
	struct buffer_struct *send_buffer;

	send_buffer = command_builder_external_helper( false, NULL );

	readRemoteEnergyAllocation( send_buffer );

	return;
}

void readRemoteEnergyAllocation( struct buffer_struct *send_buffer )
{
	command_builder2( send_buffer, "Read", "EnAl" );

	return;
}

void com_command_setRemoteEnergyAllocation( void )
{
	struct buffer_struct *send_buffer;

	send_buffer = command_builder_external_helper( false, NULL );

	setRemoteEnergyAllocation( send_buffer );
	setRemoteEnergyAllocation( send_buffer );

	return;
}

void setRemoteEnergyAllocation( struct buffer_struct *send_buffer )
{
	char powerAllocatedBuf[BUF_SIZE_LONG];

	ltoa( powerAllocatedBuf, energyAllocated_global, 10 );

	command_builder3( send_buffer, "Set", "EnAl", powerAllocatedBuf );

	return;
}

void com_command_setRemoteAllocationAdd( void )
{
	struct buffer_struct *send_buffer;

	send_buffer = command_builder_external_helper( false, NULL );

	setRemoteAllocationAdd( send_buffer );
	setRemoteAllocationAdd( send_buffer );

	return;
}

void setRemoteAllocationAdd( struct buffer_struct *send_buffer )
{
	char buf[BUF_SIZE_LONG];

	ltoa( buf, emerAllocSend_global, 10 );
	command_builder3( send_buffer, "Set", "AllAdd", buf );

	return;
}

void com_command_readRemoteAlarm( )
{
	struct buffer_struct *send_buffer;

	send_buffer = command_builder_external_helper( false, NULL );

	readRemoteAlarm( send_buffer );

	return;
}

void readRemoteAlarm( struct buffer_struct *send_buffer )
{

	command_builder2( send_buffer, "Read", "Alarm" );

	return;
}

void com_command_setRemoteAlarm( void )
{
	struct buffer_struct *send_buffer;

	send_buffer = command_builder_external_helper( false, NULL );

	setRemoteAlarm( send_buffer );
	setRemoteAlarm( send_buffer );

	return;
}

void setRemoteAlarm( struct buffer_struct *send_buffer )
{

	char audibleAlarmBuf[4];

	int alarm1PercentThresholdTemp;
	int alarm2PercentThresholdTemp;
	char alarm1PercentThresholdTempBuf[BUF_SIZE_INT];
	char alarm2PercentThresholdTempBuf[BUF_SIZE_INT];

	fillOnOff( audibleAlarmBuf, audibleAlarm_global );

	// using itoa() - variable type is char, make sure it is an int
	alarm1PercentThresholdTemp = alarm1PercentThreshold_global;
	alarm2PercentThresholdTemp = alarm2PercentThreshold_global;
	itoa( alarm1PercentThresholdTempBuf, alarm1PercentThresholdTemp, 10 );
	itoa( alarm2PercentThresholdTempBuf, alarm2PercentThresholdTemp, 10 );

	command_builder5( send_buffer, "Set", "Alarm", audibleAlarmBuf, alarm1PercentThresholdTempBuf, alarm2PercentThresholdTempBuf );

	return;
}

void com_command_readRemotePassword( void )
{
	struct buffer_struct *send_buffer;

	send_buffer = command_builder_external_helper( false, NULL );

	readRemotePassword( send_buffer );

	return;
}

void readRemotePassword( struct buffer_struct *send_buffer )
{
	command_builder2( send_buffer, "Read", "Pass" );

	return;
}

void com_command_setRemotePassword( void )
{
	struct buffer_struct *send_buffer;

	send_buffer = command_builder_external_helper( false, NULL );

	setRemotePassword( send_buffer );
	setRemotePassword( send_buffer );

	return;
}

void setRemotePassword( struct buffer_struct *send_buffer )
{
	char passwordTemp[7];

	passwordTemp[0] = passwordSet_global[0];
	passwordTemp[1] = passwordSet_global[1];
	passwordTemp[2] = passwordSet_global[2];
	passwordTemp[3] = passwordSet_global[3];
	passwordTemp[4] = passwordSet_global[4];
	passwordTemp[5] = passwordSet_global[5];
	passwordTemp[6] = CHAR_NULL;

	command_builder3( send_buffer, "Set", "Pass", passwordTemp );

	return;
}

void com_command_readRemoteEmergency( void )
{
	struct buffer_struct *send_buffer;

	send_buffer = command_builder_external_helper( false, NULL );

	readRemoteEmergency( send_buffer );

	return;
}

void readRemoteEmergency( struct buffer_struct *send_buffer )
{
	command_builder2( send_buffer, "Read", "Emer" );

	return;
}

void com_command_setRemoteEmergency( void )
{
	struct buffer_struct *send_buffer;

	send_buffer = command_builder_external_helper( false, NULL );

	setRemoteEmergency( send_buffer );
	setRemoteEmergency( send_buffer );

	return;
}

void setRemoteEmergency( struct buffer_struct *send_buffer )
{
	char emerButtonEnableBuf[4];
	char emerButtonEnergyAllocateBuf[BUF_SIZE_INT];

	fillOnOff( emerButtonEnableBuf, emerButtonEnable_global );
	itoa( emerButtonEnergyAllocateBuf, emerButtonEnergyAllocate_global, 10 );

	command_builder4( send_buffer, "Set", "Emer", emerButtonEnableBuf, emerButtonEnergyAllocateBuf );

	return;
}

void com_command_readRemoteResetTime( void )
{
	struct buffer_struct *send_buffer;

	send_buffer = command_builder_external_helper( false, NULL );

	readRemoteResetTime( send_buffer );

	return;
}

void readRemoteResetTime( struct buffer_struct *send_buffer )
{
	command_builder2( send_buffer, "Read", "RstTim" );

	return;
}

void com_command_setRemoteResetTime( void )
{
	struct buffer_struct *send_buffer;

	send_buffer = command_builder_external_helper( false, NULL );

	setRemoteResetTime( send_buffer );
	setRemoteResetTime( send_buffer );

	return;
}

void setRemoteResetTime( struct buffer_struct *send_buffer )
{

	int resetTimeHourTemp;
	int resetTimeMinuteTemp;

	char resetTimeHourBuf[BUF_SIZE_INT];
	char resetTimeMinuteBuf[BUF_SIZE_INT];


	resetTimeHourTemp = resetTimeHour_global;
	resetTimeMinuteTemp = resetTimeMinute_global;

	itoa( resetTimeHourBuf, resetTimeHour_global, 10 );
	itoa( resetTimeMinuteBuf, resetTimeMinute_global, 10 );

	command_builder4( send_buffer, "Set", "RstTim", resetTimeHourBuf, resetTimeMinuteBuf );

	return;
}

void com_command_readRemoteRelay( void )
{
	struct buffer_struct *send_buffer;

	send_buffer = command_builder_external_helper( false, NULL );

	readRemoteRelay( send_buffer );

	return;
}

void readRemoteRelay( struct buffer_struct *send_buffer )
{
	command_builder2( send_buffer, "Read", "Relay" );

	return;
}

void com_command_setRemoteRelay( void )
{
	struct buffer_struct *send_buffer;

	send_buffer = command_builder_external_helper( false, NULL );

	setRemoteRelay( send_buffer );
	setRemoteRelay( send_buffer );

	return;
}

void setRemoteRelay( struct buffer_struct *send_buffer )
{
	char relayModeBuf[5];

	// relay can be
	//  0   =   off
	//  1   =   on
	//  2   =   auto
	switch( relayMode_global )
	{
		case 0:
			relayModeBuf[0] = 'O';
			relayModeBuf[1] = 'f';
			relayModeBuf[2] = 'f';
			relayModeBuf[3] = CHAR_NULL;
			relayModeBuf[4] = CHAR_NULL;
			break;

		case 1:
			relayModeBuf[0] = 'O';
			relayModeBuf[1] = 'n';
			relayModeBuf[2] = CHAR_NULL;
			relayModeBuf[3] = CHAR_NULL;
			relayModeBuf[4] = CHAR_NULL;
			break;

		case 2:
			relayModeBuf[0] = 'A';
			relayModeBuf[1] = 'u';
			relayModeBuf[2] = 't';
			relayModeBuf[3] = 'o';
			relayModeBuf[4] = CHAR_NULL;
			break;
	}

	command_builder3( send_buffer, "Set", "Relay", relayModeBuf );

	return;
}

void com_command_readRemoteStat( void )
{
	struct buffer_struct *send_buffer;

	send_buffer = command_builder_external_helper( false, NULL );

	readRemoteStat( send_buffer );

	return;
}

void readRemoteStat( struct buffer_struct *send_buffer )
{
	command_builder2( send_buffer, "Read", "Stat" );

	return;
}

void com_command_readRemotePowerFailTimes( void )
{
	struct buffer_struct *send_buffer;

	send_buffer = command_builder_external_helper( false, NULL );

	readRemotePowerFailTimes( send_buffer );

	return;
}

void readRemotePowerFailTimes( struct buffer_struct *send_buffer )
{
	command_builder2( send_buffer, "Read", "PwrFail" );

	return;
}

void com_command_readRemotePowerData( void )
{
	struct buffer_struct *send_buffer;

	send_buffer = command_builder_external_helper( false, NULL );

	readRemotePowerData( send_buffer );

	return;
}

void readRemotePowerData( struct buffer_struct *send_buffer )
{
	command_builder2( send_buffer, "Read", "PwrData" );

	return;
}


//TODO Fix Functions
// These functions are retained because they are called from within the menu system
// need to figure out what they are intended to do and make them do it

void com_command_readRemoteEnergy( void )
{

	return;
}

void com_command_doReset( void )
{

	return;
}

void initUART( void )
{
	TRISBbits.TRISB1 = 1; // U2RX
	TRISBbits.TRISB0 = 0; // U2TX


	// do the math one step at a time
	// to explicitly control the order of operations
	// otherwise we could have a overflow or underflow
	// without really knowing it
	unsigned long tempBaud;


	tempBaud = BAUD_UART2 * 16ul; // must explicitly make unsigned long
	tempBaud = FCY / tempBaud;
	tempBaud = tempBaud - 1;

	U2BRG = tempBaud;


	U2MODEbits.USIDL = 0b0;
	U2MODEbits.IREN = 0b0;
	U2MODEbits.RTSMD = 0b1;
	U2MODEbits.UEN = 0b00;
	U2MODEbits.WAKE = 0b0;
	U2MODEbits.LPBACK = 0b0;
	U2MODEbits.ABAUD = 0b0;
	U2MODEbits.RXINV = 0b0;
	U2MODEbits.BRGH = 0b0;
	U2MODEbits.PDSEL = 0b00;
	U2MODEbits.STSEL = 0b0;

	// we do not need the interrupt on receive
	// set these bits to just make sure they are in a known state
	// interesting that the UTXISEL does not exist - we need to set each bit separately
	U2STAbits.UTXISEL0 = 0b0; // transmit interrupt
	U2STAbits.UTXISEL1 = 0b0; // Must be 0b0 (large integer implicitly truncated to unsigned type)

	U2STAbits.UTXINV = 0b0;
	U2STAbits.UTXBRK = 0b0;
	U2STAbits.URXISEL = 0b00; // interrupt after one RX character is received
	U2STAbits.ADDEN = 0;

	// we need the error interrupt to clear errors - without this it does not work
	IFS4bits.U2ERIF = 0; // clear Error Flag
	IEC4bits.U2ERIE = 1; // enable Error Interrupt

	IFS1bits.U2TXIF = 0; // clear TX Flag
	IEC1bits.U2TXIE = 0; // disable TX Interrupt

	IFS1bits.U2RXIF = 0; // clear RX interrupt flag
	IEC1bits.U2RXIE = 1; // enable RX interrupt

	U2MODEbits.UARTEN = 0b1; // turn it on
	U2STAbits.UTXEN = 0b1; // enable transmit

	return;
}

void __attribute__( ( __interrupt__, __no_auto_psv__ ) ) _U2RXInterrupt( void )
{
	// we need to read all of the characters out of the RX hardware buffer
	// typically there will be only one, but there could be more
	// if the code is not written well
	// since it is easy to read all of them we just do it

	// we create our own buffer because there are significant portions
	// of the code that are blocking and it may take more than 4ms to get
	// to the communications code to process a received character
	// 9600 baud ~ 1000 cps (1ms per character)


	unsigned char tempChar;

	U2STAbits.OERR = 0;

	while( U2STAbits.URXDA == 1 )
	{

		tempChar = U2RXREG;

		uartBufferLarge_module[ uartBufferLargeCount_module] = tempChar;
		uartBufferLargeCount_module++;
		if( uartBufferLargeCount_module >= UART_LARGE_BUFFER_SIZE )
		{
			uartBufferLargeCount_module = ( UART_LARGE_BUFFER_SIZE - 1 );
		}

	}

	_U2RXIF = 0; // clear interrupt flag

	return;
}

void __attribute__( ( __interrupt__, __no_auto_psv__ ) ) _U2ErrInterrupt( void )
{
	//    U2MODEbits.UARTEN = 0b0;
	//    delayMS(1);
	//    U2MODEbits.UARTEN = 0b1;
	U2STAbits.PERR;
	U2STAbits.FERR;
	U2STAbits.OERR = 0;

	_U2ERIF = 0; // clear error interrupt flag

	return;
}
