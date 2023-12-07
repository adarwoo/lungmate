/**
 *@ingroup console
 *@defgroup console_test Unit test
 *@{
 *@file
 *****************************************************************************
 * Unit test the console.
 * A console is brought up for this test.
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

#include "wgx.h"
#include "uart.h"
#include "console.h"
#include "nvParam.h"

int main(void)
{
   // Initialise the parameter storage in eeprom
   nvParamInit();

   // Initialise the serial comms
   uartInit(NULL);

   sei();

   // Never exit
   for (;;)
   {
      uartPrintln("Starting console...");

	   // Start a console session
      consolePrompt();

      uartPrintln("Exiting console...");
   }

   return 0;
}

