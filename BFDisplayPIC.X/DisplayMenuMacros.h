/* 
 * File:   DisplayMenuMacros.h
 * Author: Austin
 *
 * Created on May 29, 2019, 9:07 AM
 */

/*
 This file exists because these macros are used in two C files
    DisplayMain.c
    DisplayMenu.c
  
 */

#ifndef DISPLAYMENUMACROS_H
#define	DISPLAYMENUMACROS_H

#define FIRST_LINE    0b00000010
#define SECOND_LINE   0b11000000
#define THIRD_LINE    0b10010100
#define FOURTH_LINE   0b11010100
#define CLEAR_DISPLAY 0b00000001

#define NUM_LCD_LINES 4
#define NUM_LCD_WIDTH 20

// Menu defines
#define MENU_SAME               0
#define MENU_HOME_BASIC         1
#define MENU_HOME_DETAIL        2
#define MENU_ALARM              3
#define MENU_DEBUG              4
#define MENU_MAIN_1             5
#define MENU_MAIN_2             6
#define MENU_MAIN_3             7
#define MENU_MAIN_4             8
#define MENU_MAIN_5             9
#define MENU_MAIN_6            10
#define MENU_ALARM_1           11
#define MENU_ALARM_2           12
#define MENU_ALARM_3           13
#define MENU_ALARM_4           14
#define MENU_ALARM_5           15
#define MENU_ALARM_6           16
#define MENU_ALARM_7           17
#define MENU_ABOUT             18
#define MENU_ADMIN_LOGIN       19
#define MENU_ADMIN_1           20
#define MENU_ADMIN_2           21
#define MENU_ADMIN_3           22
#define MENU_ADMIN_4           23
#define MENU_ADMIN_5           24
#define MENU_ADMIN_6           25
#define MENU_SET_TIME          26
#define MENU_SET_POWER         27
#define MENU_SET_RESET_TIME    28
#define MENU_EMERGENCY_1       29
#define MENU_EMERGENCY_2       30
#define MENU_EMERGENCY_3       31
#define MENU_EMERGENCY_4       32
#define MENU_PASSWORD_CHANGE   33
#define MENU_HIGH_LOW          34
#define MENU_SET_RELAY         35
#define MENU_STATISTICS        36
#define MENU_POWERFAILTIMES    37
#define MENU_BAD_DATE         100
#define MENU_SHUT_OFF_WARNING 101
#define MENU_POWER_RESET      200
#define MENU_404              255

#endif	/* DISPLAYMENUMACROS_H */

