#ifndef DISPLAYRTCC_H
#    define	DISPLAYRTCC_H


extern unsigned char timeYear_global;
extern unsigned char timeMonth_global;
extern unsigned char timeDay_global;

extern unsigned char timeHour_global;
extern unsigned char timeMinute_global;
extern unsigned char timeSecond_global;


// not sorted yet
void rtccReadTime( void );
char writeTime( char newYear, char newMonth, char newDay, char newHour, char newMinute, char newSecond );

extern int resetTimeHour_global;
extern int resetTimeMinute_global;



extern char timeSetPos_global;
extern char tempHour_global;
extern char tempMin_global;
extern char tempMonth_global;
extern char tempDay_global;
extern char tempYear_global;

#endif	/* DISPLAYRTCC_H */

