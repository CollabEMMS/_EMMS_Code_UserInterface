#ifndef DISPLAYMENU_H
#define	DISPLAYMENU_H


extern char audibleAlarm;
extern char alarm1Enabled;
extern char alarm2Enabled;
extern char alarm1Energy;
extern char alarm2Energy;

extern char passwordSet[6];

extern int resetTimeHour;
extern int resetTimeMinute;
extern int resetTimeSecond;

extern long totalUsed;
extern long previousDayUsed;

extern char powerDownTime[12];
extern char powerUpTime[12];


// not sorted yet
extern char emerButtonEnable;
extern int  emerButtonEnergyAllocate;
extern unsigned char tempResetHour;
extern unsigned char tempResetMinute;
extern char relayActive;

extern int  emerButtonEnergyAllocate;
extern int  emerAllocNow;
extern int emerAllocSend;

extern char powerBoxCodeVersionString[9];

extern unsigned char menuState;
extern unsigned char oldMenuState;
extern unsigned char enablePeriodicUpdate;

extern char activeAlarm;
extern char silenceAlarmOne;
extern char silenceAlarmTwo;
extern char tempPercent;
extern char alarmToResume;
extern char remainingSets;

extern unsigned long powerLoad;
extern unsigned long energyUsed;
extern long energyAllocated;
extern unsigned long lowAlloc;
extern unsigned long highAlloc;
extern unsigned long tempAlloc;

extern char percentRem;

extern unsigned char button0Flag;
extern unsigned char button1Flag;
extern unsigned char button2Flag;
extern unsigned char button3Flag;

extern char relayActive;
extern char tempRelayActive;

extern char isBooting;

extern char alarmOneHit;
extern char alarmTwoHit;



void initDisplay(void);
void writeToDisplay(const char*, unsigned char, char);

void calcPercentRem( void );
void calcTimeRemaining( void );
void updateMenu( void );


#endif	/* DISPLAYMENU_H */

