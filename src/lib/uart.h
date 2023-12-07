#ifndef __UART_H_HAS_ALREADY_BEEN_INCLUDED__
#define __UART_H_HAS_ALREADY_BEEN_INCLUDED__
/**
 *@ingroup lib
 *@defgroup uart Software UART API
 *@{
 *@file
 *****************************************************************************
 * Defines the uart API.
 *
 * The following macros are required in the config.h file:
 *
 *  SYS_CLOCK    defines the MPU primary clock in Hz
 *  UART_BAUD    Defines the baud rate to use.
 *
 * The following are required to enable the transmit API:
 *
 *  UART_TX_PORT Port used for the Tx pin
 *  UART_TX_BIT  Bit number of the Tx pin
 *  UART_TX_POL  Set to 1 to indicate the polarity is inverted on the Tx pin.
 *               This would be required if the Tx pin was used directly.
 *
 * The following are required to enable the receive API:
 *
 *  UART_RX_PORT Port used for the Rx pin
 *  UART_RX_BIT  Bit number of the Rx pin
 *  UART_RX_POL  Set to 1 to invert the polarity on the Rx pin.
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

#include "wgx.h"

// Check for uart definitions
#if defined UART_TX_PORT && defined UART_TX_BIT
   #define UART_HAS_TX
   #ifndef UART_TX_POL
      // Regular polarity
      #define UART_TX_POL 0
   #endif
#endif

#if defined UART_RX_PORT && defined UART_RX_BIT
   #define UART_HAS_RX
   #ifndef UART_RX_POL
      #define UART_RX_POL 0
   #endif
#endif

#if ! defined UART_HAS_TX && ! defined UART_HAS_RX
#   error "Attempt to use the uart API where no UART resources have been defined"
#endif


/** 
 * Callback type to be passed in the init method to turn off system interrupts 
 * whilst a character is received
 */
typedef void (*uartShutdownCallback_t)(void);

void uartInit( uartShutdownCallback_t callback );

// Tx API
void uartTransmit( const uint8_t c );
void uartSendChar( const unsigned char c );
void uartPrint( PGM_P str );
void uartPrintln( PGM_P str );
void uartPrintNumber( int number, int digits );

/** 
 * Sends the string through the uart with the appropriate log header 
 * @param str  String containing the message to log. The string will be made
 *              into a flash only storage
 */
#define uartLogInfo( str )   uartPrintln( PSTR(".+ " str ) )

/** Sends an string through the uart with the appropriate error header 
 * @see uartLogInfo */
#define uartLogError( str )  uartPrintln( PSTR(".# " str ) )

// Rx API
int  uartGetChar(void);
bool uartHasChar(void);


#endif   /* ndef __UART_H_HAS_ALREADY_BEEN_INCLUDED__ */

