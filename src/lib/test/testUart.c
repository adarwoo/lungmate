/**
 *@ingroup uart
 *@defgroup uart_test Unit test
 *@{
 *@file
 *****************************************************************************
 * Unit test for the uart API
 * This unit test will go over all API. At the end, the input will
 *  be echoed back.
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

#include "wgx.h"
#include "dbg.h"
#include "uart.h"


/** 
 * Entry point for the unit test 
 * @return The method should never return
 */
int main(void)
{
   int i;

   // Initialise the debug pins
   dbgInit();

   // Initialise the serial comms with no shutdown callback
   uartInit( NULL );

   // Test simply - un-interpretted send
   uartTransmit( 'a' );
   uartTransmit( 'b' );
   uartTransmit( 'c' );

   // Test simple send (interpretted)
   uartSendChar( 'd' );
   uartSendChar( 'e' );
   uartSendChar( 'f' );
   uartSendChar( 'g' );
   uartSendChar( '\n' );

   // Test print
   uartPrint( PSTR("Hello") );
   uartPrintln( PSTR(" World!") );

   // Welcome message out on the uart
   uartLogInfo( "UART Test v" REVISION );
   uartLogError( "Logging an error message...");

   // Test print numbers
   for (i=0; i<5; ++i)
   {
      uartPrintNumber(0,i);
      uartSendChar( '\n' );
      uartPrintNumber(1,i);
      uartSendChar( '\n' );
      uartPrintNumber(12,i);
      uartSendChar( '\n' );
      uartPrintNumber(123,i);
      uartSendChar( '\n' );
      uartPrintNumber(1234,i);
      uartSendChar( '\n' );
      uartPrintNumber(12345,i);
      uartSendChar( '\n' );
      uartPrintNumber(-1,i);
      uartSendChar( '\n' );
      uartPrintNumber(-12,i);
      uartSendChar( '\n' );
      uartPrintNumber(-123,i);
      uartSendChar( '\n' );
      uartPrintNumber(-1234,i);
      uartSendChar( '\n' );
      uartPrintNumber(-12345,i);
      uartSendChar( '\n' );
   }

   // Set the sleep mode to idle
   set_sleep_mode(0);

   // Enter the main loop where we wait for a uart event
   uartLogInfo( "Testing input - type a key to be echoed" );
   for (;;)
   {
      sleep_mode();

      // The serial port is receiving a new character
      if ( uartHasChar() )
      {
         // Read the character -  will block until the char has been fully received
         // This will clear the uartRxActive internal flag
         int c = uartGetChar();

         if ( c != -1 ) // Framing error
         {
            // Echo back
            uartPrint("Char:");
            uartSendChar(c);
            uartPrint(" Ascii:");
            uartPrintNumber(c,0);
            uartSendChar( '\n' );
         }
         else
         {
            uartLogError("Framing error");
         }
      }
   }

   // Unreachable !
   return 0;
}


/* ----------------------------  End of file  ---------------------------- */
