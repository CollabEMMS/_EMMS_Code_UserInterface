#include "stdbool.h"


#ifndef COMMUNICATIONS_H
#    define	COMMUNICATIONS_H


void communications( bool );

void com_command_sendModuleInfoThis( void );
void com_command_readModuleInfo( void );;

void com_command_readRemoteMeterName( void );

void com_command_readRemoteTime( void );
void com_command_setRemoteTime( void );

void com_command_readRemoteAlarm( void );
void com_command_setRemoteAlarm( void );

void com_command_readRemotePassword( void );
void com_command_setRemotePassword( void );

void com_command_readRemoteEmergency( void );
void com_command_setRemoteEmergency( void );

void com_command_readRemoteResetTime( void );
void com_command_setRemoteResetTime( void );

void com_command_readRemoteRelay( void );
void com_command_setRemoteRelay( void );

void com_command_readRemoteStat( void );

void com_command_readRemotePowerFailTimes( void );

void com_command_readRemotePowerData( void );

void com_command_setRemoteAllocationAdd( void );

void com_command_readRemoteEnergyAllocation( void );
void com_command_setRemoteEnergyAllocation( void );

bool commBufferEmpty( void );

#endif

