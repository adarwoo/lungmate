/**
 *@ingroup lib
 *@defgroup console Console API
 *@{
 *@file
 *****************************************************************************
 * Console which allow modifying the configuration parameters in eeprom
 *  over the serial port.
 * The interface has beem designed to be machine friendly.
 * This API requires the uart and nv modules.
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "wgx.h"
#include "uart.h"
#include "nvParam.h"
#include "console.h"


/** Display a list of managed parameters */
void displayParamList(void)
{
   size_t index = 0;

   for (index=0; index<nvGetSize(); ++index)
   {
      int padLength;
      nvParamData_t min, max;

      PGM_P label = nvGetRange(index, &min, &max);

      uartPrintNumber(index+1, 2);
      uartSendChar(' ');

      // Pad the label
      for ( padLength = 30 - strlen_P(label); padLength > 0; --padLength )
      {
         uartSendChar(' ');
      }

      uartPrint(label);
      uartPrint( PSTR(" = ") );
      uartPrintNumber(nvParam(index), 5);
      uartPrint( PSTR("  [") );
      uartPrintNumber(min, 0);
      uartSendChar(':');
      uartPrintNumber(max, 0);
      uartPrintln( PSTR("]") );
   }

   uartPrintln( PSTR("\n 0 exit") );
}


/**
 * Read a single argument from the uart and only accept correct characters
 *
 * @param buf Buffer receivign the value
 * @paran bufSize Max size of the buffer including the '\0' terminating character
 * @param type Type to read. Characters will be filtered out in
 * consequence
 */
static bool readIntLine(char buf[], size_t bufSize)
{
   int c;
   uint8_t index = 0;
   bool bReject;

   while ( (c = uartGetChar()) != CR && c != LF )
   {
      //  Assume good
      bReject = false;

      // Read 1 char at a time
      if (c > 0)
      {
         // Handle the backspace key
         if ( c == BS && index > 0 )
         {
            // Delete the char on screen
            uartSendChar(BS);
            uartSendChar(' ');
            uartSendChar(BS);

            // Move the index back
            --index;

            // Delete in the buffer (chop the string)
            buf[index] = 0;

            // Wait for another char
            continue;
         }

         // Check whether to accept more chars (include the 0)
         if (index >= (bufSize - 1))
         {
            bReject = true;
         }
         else
         {
            if ( ! ( isdigit(c) || ( (c == '-') && (index == 0) ) ) )
            {
               bReject = true;
            }
         }

         // Reject and beep or pring the char
         if (bReject)
         {
            uartSendChar(BEL);
         }
         else
         {
            uartSendChar(c);
            // Store the character
            buf[index] = c;
            // Terminate the string
            buf[++index] = '\0';
         }
      }
   }

   uartSendChar('\n');

   return index > 0;
}


/**
 * Helper which reads from the uart and convert back to an int
 *
 * @param value  Integer pointer to receive the result
 */
static bool readInt( int *value )
{
   bool retval = false;

   char buf[ CONSOLE_MAX_CHARACTERS_IN_INT16 + 1 /*zero at end*/ ];

   if ( readIntLine(buf, sizeof(buf)) )
   {
      // Decode the string
      *value = atoi(buf);

      retval = true;
   }

   return retval;
}


/**
 * Called to switch in console mode after a startbit was
 * detected on the Rx pin
 */
void consolePrompt()
{
   size_t index;

   uartPrintln( PSTR("# LungMate v" REVISION ) );
   displayParamList();

   for (;;)
   {
      // Display the PS prompt
      uartPrint( PSTR("\n> ") );

      // Get the param index to modify
      if (readInt((int*)(&index)))
      {
         // Parse the param
         if (index == 0)
         {
            // Exit console mode
            break;
         }
         else if (index > 0 && index < nvEndOfParamEnum_e )
         {
            nvSetStatus_t status = nvOK_e;

            // The displayed index if one indexed - adjust accordingly
            --index;

            nvParamData_t newValue = nvParamGetValue(index);

            uartPrintNumber( newValue, 0 );
            uartPrint( PSTR(" < ") );

            if ( readInt((int*)&newValue) )
            {
               status = nvParamSet(index, newValue);
            }

            switch (status)
            {
            case nvOK_e:
               // Nothing to do
               break;
            case nvIndexOutOfRange_e:
               consoleError("Out of range");
               break;
            case nvNumberTooLarge_e:
               consoleError("Too big");
               break;
            case nvNumberTooSmall_e:
               consoleError("Too small");
               break;
            }
         }
      }
      else
      {
         // Display the param list
         displayParamList();
      }
   }
}


/* ----------------------------  End of file  ---------------------------- */

