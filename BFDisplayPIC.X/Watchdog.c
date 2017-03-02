/* File:    Watchdog.c
 * Author: Nathan Chaney
 */

/* Includes *******************************************************************/

#include "ExternSharedDefinitions.h"
#include <p24FV32KA301.h>

/* Functions ******************************************************************/

void resetWDT(void) {
    asm("CLRWDT");
}