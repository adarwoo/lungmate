/**
 *@ingroup fft
 *@defgroup fft_test Unit test
 *@{
 *@file
 *****************************************************************************
 * Unit test for the fft library.
 * Some dummy data is generated and injected into the fft algo.
 * This unit test is self testing.
 * The uart is used to displat the tests results.
 *
 * @author software@arreckx.com
 *****************************************************************************
 */
#include <math.h>
#include <stdio.h>
#include "wgx.h"
#include "fft.h"
#include "uart.h"

#if defined __CYGWIN__ || __MINGW__
#define printResult(r) printf(".+ Result: %f\n", r)
#else
#define printResult(r)
#endif

static void halt(void)
{
   set_sleep_mode(0);

   for (;;)
   {
      sleep_mode();
   }
}

int main(void)
{
   int16_t data;
   size_t i;
   static const long centerFreq = 50;

   uartInit(NULL);

   uartLogInfo("TEST : fftInit");

   // Put the data through the fft
   fftInit( centerFreq );

   uartLogInfo("TEST : no signal");

   // No signal - on power
   for ( i=0; ; ++i )
   {
      data = 0;

      if ( i > 128 )
      {
         uartLogError("FAILED : Enough data supplied did not yeild a result");
         halt();
      }

      if ( fftNext( data ) )
      {
         float res = fftGetResult();
         printResult( res );

         if ( res < -1.0 || res > 1.0 )
         {
            uartLogError("FAILED : result out of range");
            halt();
         }

         break;
      }
   }

   uartLogInfo("TEST : fftReset");

   fftReset();

   uartLogInfo("TEST : simple sine");

   for ( i=0; ; ++i )
   {
      data = (int16_t)( 100.0 * sin( 2.0 * M_PI * (float)(i*centerFreq) / (float)ADC_SAMPLE_FREQUENCY ));

      if ( fftNext( data ) )
      {
         float res = fftGetResult();
         printResult( res );

         if ( res < 99.0 || res > 101.0 )
         {
            uartLogError("FAILED : result out of range");
            halt();
         }

         break;
      }
   }

   fftReset();

   uartLogInfo("TEST : DC Offset");

   // DC Offset
   for ( i=0; ; ++i )
   {
      data = 20 + (int16_t)( 100.0 * sin( 2.0 * M_PI * (float)(i*centerFreq) / (float)ADC_SAMPLE_FREQUENCY ));

      if ( fftNext( data ) )
      {
         float res = fftGetResult();
         printResult( res );

         if ( res < 99.0 || res > 101.0 )
         {
            uartLogError("FAILED : result out of range");
            halt();
         }

         break;
      }
   }

   fftReset();

   uartLogInfo("TEST : phase offset");

   // Phase offset
   for ( i=0; ; ++i )
   {
      data = (int16_t)( 100.0 * sin( 2.0 * M_PI * (float)((i+10)*centerFreq) / (float)ADC_SAMPLE_FREQUENCY ));

      if ( fftNext( data ) )
      {
         float res = fftGetResult();
         printResult( res );

         if ( res < 99.0 || res > 101.0 )
         {
            uartLogError("FAILED : result out of range");
            halt();
         }

         break;
      }
   }

   fftReset();

   uartLogInfo("TEST : different frequency over");

   // Different frequency
   for ( i=0; ; ++i )
   {
      data = (int16_t)( 100.0 * sin( 2.0 * M_PI * (float)(i*(centerFreq+10)) / (float)ADC_SAMPLE_FREQUENCY ));

      if ( fftNext( data ) )
      {
         float res = fftGetResult();
         printResult( res );

         if ( res < -1.0 || res > 1.0 )
         {
            uartLogError("FAILED : result out of range");
            halt();
         }

         break;
      }
   }

   fftReset();

   uartLogInfo("TEST : different frequency under");

   // Different frequency
   for ( i=0; ; ++i )
   {
      data = (int16_t)( 100.0 * sin( 2.0 * M_PI * (float)(i*(centerFreq-10)) / (float)ADC_SAMPLE_FREQUENCY ));

      if ( fftNext( data ) )
      {
         float res = fftGetResult();
         printResult( res );

         if ( res < -1.0 || res > 1.0 )
         {
            uartLogError("FAILED : result out of range");
            halt();
         }

         break;
      }
   }

   fftReset();

   uartLogInfo("TEST : Combined frequencies with DC offset and phase shift");

   // Combined frequencies with different sizes, offset and phase
   for ( i=0; ; ++i )
   {
      data = (int16_t)( 14 + 100.0 * sin( 2.0 * M_PI * (float)((i+4)*(centerFreq)) / (float)ADC_SAMPLE_FREQUENCY ));
      data += (int16_t)( 47 - 100.0 * sin( 2.0 * M_PI * (float)(i*(centerFreq+20)) / (float)ADC_SAMPLE_FREQUENCY ));

      if ( fftNext( data ) )
      {
         float res = fftGetResult();
         printResult( res );

         if ( res < 99.0 || res > 101.0 )
         {
            uartLogError("FAILED : result out of range");
            halt();
         }

         break;
      }
   }

   uartLogInfo("PASS");

   halt();

   return 0;
}

