/* File:    DisplayDefinitions.h
 * Authors: Dan Baker
 *          Nathan Chaney
 */

#ifndef DISPLAYDEFINITIONS_H
#define	DISPLAYDEFINITIONS_H

/* Constant Definitions *******************************************************/
//#define DISPLAY_BOX
//#define FOSC       32000000
//#define _XTAL_FREQ 32000000
//#define MENU_HOME_BASIC          1 // from display header
//#define MENU_HOME_DETAIL         2


/* Function Definitions *******************************************************/

//void init(void);
//void initVars(void);
//void initPorts(void);
//void enablePullDownResistors(void);
//void enableInterrupts(void);
//void disableInterrupts(void);
//void enableAlarm(void);
//void startAlarm(void);
//void checkCommFailure(void);
//void initRTCCDisplay(void);
//void initTimer(void);

/* Variable Declarations ******************************************************/

// Code version format:               YYMMDDVV (VV is version)
//unsigned long displayBoxCodeVersion = 15042801;
//unsigned char menuState;
//unsigned char button0Flag, button1Flag, button2Flag, button3Flag;
//unsigned char resetTime;
//char alarmPulse;
//char alarmEnd;

/* DisplayMenu.c **************************************************************/
/* Constant Definitions *******************************************************/

//#define FIRST_LINE    0b00000010
//#define SECOND_LINE   0b11000000
//#define THIRD_LINE    0b10010100
//#define FOURTH_LINE   0b11010100
//#define CLEAR_DISPLAY 0b00000001
//
//#define NUM_LCD_LINES 4
//#define NUM_LCD_WIDTH 20
//
//// Menu defines
//#define MENU_SAME               0
//#define MENU_HOME_BASIC         1
//#define MENU_HOME_DETAIL        2
//#define MENU_ALARM              3
//#define MENU_DEBUG              4
//#define MENU_MAIN_1             5
//#define MENU_MAIN_2             6
//#define MENU_MAIN_3             7
//#define MENU_MAIN_4             8
//#define MENU_MAIN_5             9
//#define MENU_MAIN_6            10
//#define MENU_ALARM_1           11
//#define MENU_ALARM_2           12
//#define MENU_ALARM_3           13
//#define MENU_ALARM_4           14
//#define MENU_ALARM_5           15
//#define MENU_ALARM_6           16
//#define MENU_ALARM_7           17
//#define MENU_ABOUT             18
//#define MENU_ADMIN_LOGIN       19
//#define MENU_ADMIN_1           20
//#define MENU_ADMIN_2           21
//#define MENU_ADMIN_3           22
//#define MENU_ADMIN_4           23
//#define MENU_ADMIN_5           24
//#define MENU_ADMIN_6           25
//#define MENU_SET_TIME          26
//#define MENU_SET_POWER         27
//#define MENU_SET_RESET_TIME    28
//#define MENU_EMERGENCY_1       29
//#define MENU_EMERGENCY_2       30
//#define MENU_EMERGENCY_3       31
//#define MENU_EMERGENCY_4       32
//#define MENU_PASSWORD_CHANGE   33
//#define MENU_HIGH_LOW          34
//#define MENU_SET_RELAY         35
//#define MENU_STATISTICS        36
//#define MENU_POWERFAILTIMES    37
//#define MENU_BAD_DATE         100
//#define MENU_SHUT_OFF_WARNING 101
//#define MENU_POWER_RESET      200
//#define MENU_404              255


/* Function Definitions *******************************************************/

//void dspCommand(unsigned char);
//void dspWrite(unsigned char);
//void dspNibble(void);
//void initDisplay(void);
//void dspSetP1(unsigned char);
//void writeToDisplay(const char*, unsigned char, char);
//void dspWriteMessage(char*);

//void updateMenu(void);
//void menuError(void);
//void menuDebug(void);
//void menuHomeBasic(void);
//void menuHomeDetail(void);
//void menuAlarm(void);
//void menuMain1(void);
//void menuMain2(void);
//void menuMain3(void);
//void menuMain4(void);
//void menuMain5(void);
//void menuMain6(void);
//void menuAlarm1(void);
//void menuAlarm2(void);
//void menuAlarm3(void);
//void menuAlarm4(void);
//void menuAlarm5(void);
//void menuAlarm6(void);
//void menuAlarm7(void);
//void menuAbout(void);
//void menuAdminLogin(void);
//void menuAdmin1(void);
//void menuAdmin2(void);
//void menuAdmin3(void);
//void menuAdmin4(void);
//void menuAdmin5(void);
//void menuAdmin6(void);
//void menuSetTime(void);
//void menuSetPower(void);
//void menuSetResetTime(void);
//void menuStatistics(void);
//void menuPowerFailTimes(void);
//void menuEmergency1(void);
//void menuEmergency2(void);
//void menuEmergency3(void);
//void menuEmergency4(void);
//void menuHighLow(void);
//void menuSetRelay(void);
//void menuBadDate(void);
//void menuShutOffWarning(void);
//void menuPasswordChange(void);
//void menuPowerReset(void);
//void writeBarGraph(void);
//void calcPercentRem(void);

//void calcTimeRemaining(void);

//char menuButtonRead(char menu1, char menu2, char menu3, char menu4);

/* Variable Definitions *******************************************************/

//unsigned char lowNibble, highNibble;
//char currentDisplay[NUM_LCD_LINES * NUM_LCD_WIDTH + 1];
//char nextDisplay[NUM_LCD_LINES * NUM_LCD_WIDTH + 1];
//char buffer[20];

//unsigned char menuState;
//unsigned char oldMenuState;
//unsigned char lastUpdateSecond;
//unsigned char lastUpdateMinute;
//char enablePeriodicUpdate;

//char passwordInput[6];
//char newPassword[12];
//unsigned char pwLength;

//char remainingSets;
//char silenceAlarmOne;
//char silenceAlarmTwo;
//char activeAlarm;
//char alarmToResume;
//char numBeeps;
//char numSets;
//char rightArrow[2];
//char tempPercent;
//int  emerButtonAllocTemp;

//unsigned long currentLoad;
//unsigned long powerUsed;
//unsigned long powerAllocated;
//unsigned long lowAlloc;
//unsigned long highAlloc;
//unsigned long tempAlloc;
//char percentRem;
//char homeBasicAlternate; // 0 for % remaining, 1 for % load
//char barGraph[21];

//unsigned char button0Flag, button1Flag, button2Flag, button3Flag;

//unsigned char tempResetHour;
//unsigned char tempResetMinute;
//char tempResetTimeString[6];

//char tempIsHigh, oldPowerMenuState;

//unsigned char timeRemHour, timeRemMinute, timeRemSecond, timeRemUpdate;
//unsigned long timeRem1, timeRem2, timeRem3, timeRem4, timeRem5;
//char timeRemainingString[9];

//char tempRelayActive;


//char isBooting;
//char alarmOneHit, alarmTwoHit;

/* RTCC Display ***************************************************************/
//void initRTCCDisplay(void);

#endif