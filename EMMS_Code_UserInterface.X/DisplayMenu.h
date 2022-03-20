#ifndef DISPLAYMENU_H
#define	DISPLAYMENU_H


extern bool audibleAlarm_global;
extern char alarm1PercentThreshold_global;
extern char alarm2PercentThreshold_global;

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
extern bool emerButtonEnable_global;
extern int emerButtonEnergyAllocate_global;
extern int tempResetHour_global;
extern int tempResetMinute_global;

extern int emerAllocNow_global;
extern int emerAllocSend_global;

extern char powerBoxCodeVersionString_global[9];

extern unsigned char menuState_global;
extern unsigned char oldMenuState_global;
extern bool enablePeriodicUpdate_global;

extern char alarmActive_global;
extern bool alarm1Silence_global;
extern bool alarm2Silence_global;
extern char tempPercent_global;
extern char alarmToResume_global;
extern char alarmRemainingSets_global;

extern unsigned long powerLoad_global;
extern unsigned long energyUsed_global;
extern unsigned long energyAllocated_global;

extern char percentRem_global;

extern unsigned char button0Flag_global;
extern unsigned char button1Flag_global;
extern unsigned char button2Flag_global;
extern unsigned char button3Flag_global;

extern char isBooting_global;

extern bool alarm1Hit_global;
extern bool alarm2Hit_global;



void initDisplay(void);
void dispUpdate( void );
void writeToDisplay(const char*, unsigned char, char);

void calcPercentRem(void);
void calcTimeRemaining(void);
void updateMenu(void);


#endif	/* DISPLAYMENU_H */

