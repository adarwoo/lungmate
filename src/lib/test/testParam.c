/**
 *@ingroup nvparam
 *@defgroup nvparam_test Unit test
 *@{
 *@file
 *****************************************************************************
 * Unit test for the non-volatile param API.
 * This unit test will pass/fail automatically.
 * It requires the uart to display results.
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

#include "wgx.h"
#include "dbg.h"
#include "nvParam.h"
#include "uart.h"

#include "string.h"


/** 
 * Entry point for the unit test 
 * @return The method should never return
 */
int main(void)
{
   dbgInit();
   uartInit(NULL);
   nvParamInit();

   uartLogInfo("TEST - nvGetSize");

   // Check the number of parameters
   if ( nvGetSize() != 4 )
   {
      uartLogError("FAILED - wrong number of params");
   }
   else
   {
      // Test resetAll
      uartLogInfo("TEST - nvParamResetAll");
      nvParamResetAll();

      uartLogInfo("TEST - nvParamGetValue");

      // Check all values reset
      if ( nvParamGetValue(paramUnsigned_e) != 9999 )
      {
         uartLogError("FAILED - paramUnsigned_e : expected 9999, got:");
         uartPrintNumber( nvParamGetValue(paramUnsigned_e), 0 );
      }
      else if ( nvParamGetValue(paramSigned_e) != 0 )
      {
         uartLogError("FAILED - paramUnsigned_e : expected 0, got");
         uartPrintNumber( nvParamGetValue(paramUnsigned_e), 0 );
      }
      else if ( nvParamGetValue(paramBool0_e) != 0 )
      {
         uartLogError("FAILED - paramBool0 : expected 0, got");
         uartPrintNumber( nvParamGetValue(paramBool0_e), 0 );
      }
      else if ( nvParamGetValue(paramBool1_e) != 1 )
      {
         uartLogError("FAILED - paramBool1 : expected 1, got");
         uartPrintNumber( nvParamGetValue(paramBool1_e), 0 );
      }
      else
      {
         nvSetStatus_t status;

         uartLogInfo("TEST - nvParamSet");
         status = nvParamSet( paramUnsigned_e, 777 );

         if ( status != nvOK_e )
         {
            uartLogError("FAILED - Bad return status : exepted 0, got:");
            uartPrintNumber( (uint8_t)status, 0 );
         }
         else if ( nvParamGetValue(paramUnsigned_e) != 777 )
         {
            // Read value back
            uartLogError("FAILED - paramUnsigned_e : expected 777, got:");
            uartPrintNumber( nvParamGetValue(paramUnsigned_e), 0 );
         }
         else
         {
            // Check bad values
            status = nvParamSet( paramBool0_e, 2 );

            if ( status != nvNumberTooLarge_e )
            {
               uartLogError("FAILED - Bad return status : exepted 2, got:");
               uartPrintNumber( (uint8_t)status, 0 );
            }
            else
            {
               // Check bad values
               status = nvParamSet( paramBool0_e, -1 );

               if ( status != nvNumberTooSmall_e )
               {
                  uartLogError("FAILED - Bad return status : exepted 3, got:");
                  uartPrintNumber( (uint8_t)status, 0 );
               }
               else
               {
                  // Check bad values
                  status = nvParamSet( nvEndOfParamEnum_e, 2 );

                  if ( status != nvIndexOutOfRange_e )
                  {
                     uartLogError("FAILED - Bad return status : exepted 1, got:");
                     uartPrintNumber( (uint8_t)status, 0 );
                  }
                  else
                  {
                     PGM_P text;
                     nvParamData_t min, max;

                     uartLogInfo("TEST - nvGetRange");

                     text=nvGetRange( paramUnsigned_e, &min, &max );

                     if ( strcmp_P(text,PSTR("Unsigned parameter")) != 0 )
                     {
                        uartLogError("FAILED - Bad returned text");
                     }
                     else if ( min != 0 )
                     {
                        uartLogError("FAILED - Bad return min : exepted 0, got:");
                        uartPrintNumber( min, 0 );
                     }
                     else if ( max != 32767 )
                     {
                        uartLogError("FAILED - Bad return max : exepted 32768, got:");
                        uartPrintNumber( max, 0 );
                     }
                     else
                     {
                        uartLogInfo("ALL PASSED");
                     }
                  }
               }
            }
         }
      }
   }

   return 0;
}

