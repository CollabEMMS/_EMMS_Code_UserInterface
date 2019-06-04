#ifndef DISPLAYRTCC_H
#define	DISPLAYRTCC_H


extern unsigned char timeYear;
extern unsigned char timeMonth;
extern unsigned char timeDay;
extern unsigned char timeWeekday;
extern unsigned char timeHour;
extern unsigned char timeMinute;
extern unsigned char timeSecond;


// not sorted yet
void readTime(void);
void writeClockStrings(void);
void writeTempClockStrings(void);
char writeTime(char newYear, char newMonth, char newDay, char newHour, char newMinute, char newSecond);

extern unsigned char resetTimeSecond;
extern unsigned char resetTimeMinute;

extern char clockStr[6];
extern char calendarStr[9];


extern char timeSetPos;
extern char tempHour, tempMin, tempMonth, tempDay, tempYear;
extern char tempClockStr[10], tempCalStr[15];

#endif	/* DISPLAYRTCC_H */

