/**
 *@ingroup adc
 *@defgroup adc_test Unit test
 *@{
 *@file
 *****************************************************************************
 * Unit test the adc converter.
 * For this test, a function generator or potentiometer should swing
 *  the analog input either side of the mid-biased input to acheive full range.
 * The raw measured value is then constantly displayed on the uart terminal.
 * At 320Hz to value should be updated every 320/256 = 1.25s
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

#include "wgx.h"
#include "dbg.h"
#include "uart.h"
#include "adc.h"

int main(void)
{
   size_t count = 0;
   uint32_t rawValue = 0;

   dbgInit();
   uartInit(NULL);
   adcInit();

   set_sleep_mode(0);

   // Enter the main loop where we wait for the adc to have a value
   // An average of 4 measurments is taken and the value is written on the serial port
   for (;;)
   {
      sleep_mode();

      if ( adcTick() )
      {
         dbgToggle(DBG_ADC);
      }
      else if ( adcHasNewValue() )
      {
         rawValue += adcGetValue();

         if ( ++count == 256 )
         {
            count=0;
            adcShutdown();
            uartPrintNumber( rawValue >> 8, 0 );
            uartSendChar('\n');
            adcInit();
            rawValue=0;
         }
      }

   }

   return 0;
}


/* ----------------------------  End of file  ---------------------------- */

