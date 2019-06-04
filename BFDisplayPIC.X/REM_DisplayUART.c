///* File:    DisplayUART.c
// * Authors: Dan Baker
// *          Nathan Chaney
// */
//
///* Includes *******************************************************************/
//
//#include <p24FV32KA301.h>
//#include "ExternSharedDefinitions.h"
//#include "DisplayUART.h"
//
///* Functions ******************************************************************/
//
//
//volatile unsigned char uartBufferLarge[ UART_LARGE_BUFFER_SIZE];
//volatile unsigned char uartBufferLargeCount = 0;
//
//void initUART( void )
//{
//    // FCY = 2 MHz;
//    // BAUD = FCY / 16 / (U2BRG + 1)
//    // Ok to be slightly off
//    // U2BRG = 1;  // baud rate is 62500
//    // U2BRG = 3;  // baud rate is 31250
//    // U2BRG = 6;  // baud rate is 17829
//    U2BRG = 12; // baud rate is 9615
//    // U2BRG = 25; // baud rate is 4800
//    // U2BRG = 50; // baud rate is 2447
//
//
//    U2MODEbits.USIDL = 0b0;
//    U2MODEbits.IREN = 0b0;
//    U2MODEbits.RTSMD = 0b1;
//    U2MODEbits.UEN = 0b00;
//    U2MODEbits.WAKE = 0b0;
//    U2MODEbits.LPBACK = 0b0;
//    U2MODEbits.ABAUD = 0b0;
//    U2MODEbits.RXINV = 0b0;
//    U2MODEbits.BRGH = 0b0;
//    U2MODEbits.PDSEL = 0b00;
//    U2MODEbits.STSEL = 0b0;
//
//    U2STAbits.UTXISEL0 = 0b0;
//    U2STAbits.UTXISEL1 = 0b0;
//    U2STAbits.UTXINV = 0b0;
//    U2STAbits.UTXBRK = 0b0;
//
//    U2STAbits.URXISEL0 = 0b00; // interrupt after one RX character is received
//    //    U2STAbits.URXISEL1 = 0b0; // interrupt after one RX character is received
//    U2STAbits.ADDEN = 0;
//
//    //interrupts
//    IPC7bits.U2RXIP = 1; // set RX interrupt priority (1-7, default 4)
//    IFS4bits.U2ERIF = 0; // clear Error Flag
//    IEC4bits.U2ERIE = 1; // enable Error Interrupt
//    IFS1bits.U2TXIF = 0; // clear TX Flag
//    IEC1bits.U2TXIE = 0; // disable TX Interrupt
//    IFS1bits.U2RXIF = 0; // clear RX interrupt flag
//    IEC1bits.U2RXIE = 1; // enable RX interrupt
//
//
//    U2MODEbits.UARTEN = 0b1; // turn it on
//    U2STAbits.UTXEN = 0b1; // enable transmit
//
//}
//
//void __attribute__( (__interrupt__, __no_auto_psv__) ) _U2RXInterrupt( void )
//{
//    // we need to read all of the characters out of the RX hardware buffer
//    // typically there will be only one, but there could be more
//    // if the code is not written well
//    // since it is easy to read all of them we just do it
//
//    // we create our own buffer because there are significant portions
//    // of the code that are blocking and it may take more than 4ms to get
//    // to the communications code to process a received character
//    // 9600 baud ~ 1000 cps (1ms per character)
//
//
//    unsigned char tempChar;
//
//    U2STAbits.OERR = 0;
//
//    while( U2STAbits.URXDA == 1 )
//    {
//
//	tempChar = U2RXREG;
//
//	uartBufferLarge[ uartBufferLargeCount] = tempChar;
//	uartBufferLargeCount++;
//	if( uartBufferLargeCount >= UART_LARGE_BUFFER_SIZE )
//	{
//	    uartBufferLargeCount = (UART_LARGE_BUFFER_SIZE - 1);
//	}
//
//    }
//
//    _U2RXIF = 0; // clear interrupt flag
//}
//
//void __attribute__( (__interrupt__, __no_auto_psv__) ) _U2ErrInterrupt( void )
//{
//    //    U2MODEbits.UARTEN = 0b0;
//    //    delayMS(1);
//    //    U2MODEbits.UARTEN = 0b1;
//    U2STAbits.PERR;
//    U2STAbits.FERR;
//    U2STAbits.OERR = 0;
//
//    _U2ERIF = 0; // clear error interrupt flag
//}
