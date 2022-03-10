#ifndef DISPLAYMENU_H
#define	DISPLAYMENU_H


extern char audibleAlarm_global;
extern char alarm1Enabled_global;
extern char alarm2Enabled_global;
extern char alarm1Energy_global;
extern char alarm2Energy_global;

extern char passwordSet_global[6];

extern int resetTimeHour_global;
extern int resetTimeMinute_global;
extern int resetTimeSecond_global;

extern long totalUsed_global;
extern long previousDayUsed_global;

extern char powerDownTime_global[12];
extern char powerUpTime_global[12];

extern char relayMode_global;
//extern char relayModeTemp_global;

// not sorted yet
extern char emerButtonEnable_global;
extern int emerButtonEnergyAllocate_global;
extern int tempResetHour_global;
extern int tempResetMinute_global;

extern int emerAllocNow_global;
extern int emerAllocSend_global;

extern char powerBoxCodeVersionString_global[9];

extern unsigned char menuState_global;
extern unsigned char oldMenuState_global;
extern unsigned char enablePeriodicUpdate_global;

extern char activeAlarm_global;
extern char silenceAlarmOne_global;
extern char silenceAlarmTwo_global;
extern char tempPercent_global;
extern char alarmToResume_global;
extern char remainingSets_global;

extern unsigned long powerLoad_global;
extern unsigned long energyUsed_global;
extern long energyAllocated_global;

extern char percentRem_global;

extern unsigned char button0Flag_global;
extern unsigned char button1Flag_global;
extern unsigned char button2Flag_global;
extern unsigned char button3Flag_global;

extern char isBooting_global;

extern char alarmOneHit_global;
extern char alarmTwoHit_global;



void initDisplay(void);
void dispUpdate( void );
void writeToDisplay(const char*, unsigned char, char);

void calcPercentRem(void);
void calcTimeRemaining(void);
void updateMenu(void);


#endif	/* DISPLAYMENU_H */

