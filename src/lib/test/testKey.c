/**
 *@ingroup key
 *@defgroup key_test Unit test
 *@{
 *@file
 *****************************************************************************
 * Test the single key API.
 * We use the ADC tick to help us scan the key.
 * The uart will display what has been pressed.
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

#include "wgx.h"
#include "dbg.h"
#include "uart.h"
#include "adc.h"
#include "key.h"

/** Callback when the key is pressed breifly */
static inline void shortPress()
{
   uartPrintln(PSTR("S"));
}

/** Callback when the key is held for a long time */
static inline void longPress()
{
   uartPrintln(PSTR("L"));
}

int main(void)
{
   size_t count = 0;
   uint16_t rawValue = 0;

   dbgInit();
   adcInit();
   uartInit(NULL);

   // Initialise the API and callback
   keyInit( shortPress, longPress );

   set_sleep_mode(0);

   // Enter the main loop where we wait for the adc to have a value
   // An average of 4 measurments is taken and the value is written on the serial port
   for (;;)
   {
      sleep_mode();

      // Use the ADC to provide the 50Hz tick
      if ( adcHasNewConversionStarted() )
      {
         // Scan the key and let the callback do the rest
         keyScan();
      }
   }

   return 0;
}
