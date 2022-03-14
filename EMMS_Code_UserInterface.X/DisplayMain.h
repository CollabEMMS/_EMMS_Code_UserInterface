/* 
 * File:   DisplayMain.h
 * Author: Austin
 *
 * Created on May 29, 2019, 6:44 AM
 */

#ifndef DISPLAYMAIN_H
#    define	DISPLAYMAIN_H

#    define DISPLAY_BOX_CODE_VERSION "20190529"

void debugBacklight( bool state );
void debugBacklightToggle( );
void debugBacklightFlash( int timeOn );

extern struct moduleInfo_struct moduleInfo_global[MODULE_COUNT];

#endif	/* DISPLAYMAIN_H */

