/**@defgroup lungmate Lungmate Dust-Exctractor Controller*/
/**@ingroup lungmate*/
/**@file
 *****************************************************************************
 * Entry point, initialisation, and main loop for this application
 * @mainpage
 * <h1>Introduction</h1>
 * Lungmate application have been written to control the dustextractor of a
 *  workshop whenever a load is detected in the mains.
 * This application has been specifically written for a Atmel ATTiny861
 *  micro-controller chip, although most of the API could be easily ported and
 *  reused on other AVR micro-controllers.
 * <h1>Overview</h1>
 * This application is mostly interrupt driven.
 *
 * Only 4 interrupts are used:
 * <ul>
 * <li>The 'timer 1 overflow' is used to drive a burst of 16 conversions
 *  at 320 bursts/sec</li>
 * <li>The 'ADC conversion complete' is used to collect oversampled results,
 *  and to compute decimated values</li>
 * <li>A level change on the serial Rx pin will cause the system to enter</li>
 *  the terminal mode which allows all parameters to be changed</li>
 * <li>The 'timer 0 16-bits compare' is used to sample serial port bits</li>
 *  </ul>
 *  The timer 1 interrupt also services the keypad, which feeds into
 *  the state machine.
 *
 * The processing of decimated values by the FFT algorithm is handled in
 *  the main program.
 *
 * The real time arrangement is such that the computation of the FFT
 *  is finished well before the next timer interrupt.
 *
 * For each new FFT result (every 64samples - 200ms), the state machine
 *  is updated and the relay status re-evaluated.
 *
 * <h1>Getting started</h1>
 * Lungmate entry point is implemented in lungmate.c main function
 *  although the initialisation phase takes place in preamble() defined in preamble.c.
 * The A/D measurements take place in adc.c.
 * The state machine (stateMachine.c) is updated by the main loop.
 * <br/>The fft coefficient factor - or twiddle factors are created by a Python
 *  script placed in utils. The resulting file is twiddle.c and should be compiled
 *  the normal way.
 * Check the following sections for reference
 * @subpage lungmate
 * @subpage lib
 * @subpage unittest
 * @subpage simulation
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

#include "wgx.h"
#include "fft.h"
#include "uart.h"
#include "console.h"
#include "dbg.h"
#include "stateMachine.h"
#include "adc.h"
#include "preamble.h"
#include "nvParam.h"
#include "key.h"


/** Mutltiplier for converting the FFT result into Watts */
static float fftToWatt;

/** Index of the next character to transmit (negative numbers to send carriage return etc.) */
static int8_t txCharIndex;

/** Stores the string value of the last know fft result (watt) */
static char acFFTResult[CONSOLE_MAX_CHARACTERS_IN_UINT16];


/**
 * Sends the fftResult down the serial link a character at a time
 *  to leave plenty of time before the next interrupt.
 * This happens after each partial fft computation.
 *
 * @return true if more characters remain to send
 */
static inline bool sendResult(void)
{
   bool retval = true;

   if ( txCharIndex >= 0 )
   {
      uartTransmit( acFFTResult[txCharIndex] );
      --txCharIndex;
   }
   else if ( txCharIndex == -1 )
   {
      uartTransmit('\n');
      --txCharIndex;
   }

   else if ( txCharIndex == -2 )
   {
      uartTransmit('\r');
      --txCharIndex;
   }
   else
   {
      retval = false;
   }

   return retval;
}


/** Process incomming traffic from the serial port */
static inline void enterConsole(void)
{
   // Read the character the incomming character
   int c = uartGetChar();

   // To enter the console mode, the first transmitted char
   //  should be a carriage return or linefeed
   if ( c == '\n' || c == '\r' )
   {
      // Finish transmitting the fft result
      while ( sendResult() )
      {
         continue;
      }

      // Enter console mode
      consolePrompt();
   }

   // Resume measurements and re-enable the interrupts that were
   //  disabled when a new character was received through a callback
   fftReset();
   adcInit();

   // Reset the index of the next char to send
   txCharIndex = INT8_MIN;
}


/** Process new adc value */
static inline void processAdcValue(void)
{
   // Compute part of the FFT and check whether this calculation has yeilded a new result
   if ( fftNext( adcGetValue() ) )
   {
      // Get the power from the FFT
      float result = fftGetResult() * fftToWatt;
      uint16_t fftResult = -1;

      // Clip at 65kw and pass to the state machine
      if ( result < 65535.0 )
      {
         // Cast to short
         fftResult = (uint16_t)result;
      }

      // Pass to the stateMachine
      smProcessFFTResult(fftResult);

      // Store the string representation in our text buffer to transmit later on
      for( txCharIndex=0; fftResult != 0 || txCharIndex == 0 ; ++txCharIndex, fftResult /= 10)
      {
         acFFTResult[txCharIndex] = (fftResult % 10) + '0';
      }

      // This is the best place to reset the watchdog
      // Reaching this point indicates all interrupts and computations
      //  are running fine. (every 200ms)
      wdt_reset();
   }
   else
   {
      sendResult();
   }
}


/**
 * Entry point of this application.
 *
 * @return 0 for ANSI compiance only. Should never ever return.
 */
int main(void)
{
   // Initialise all APIs
   preamble();

   // Read from eeprom the power conversion
   fftToWatt = nvParam( fftToWattRatio_e ) / 1000;

   // Enter the main loop where we wait for the decimation to have completed
   for (;;)
   {
      // Enter sleep. This may start an ADC conversion
      sleep_mode();

      // The debug pin will show (on an oscilloscope) the time taken to
      //  process the data in this loop. It will stay set in console mode.
      // This pin should clear before the next timer interrupt.
      dbgSet(DBG_MAIN_LOOP);

      if ( uartHasChar() ) // Is the serial port receiving a new character?
      {
         enterConsole();
      }
      else if ( adcTick() ) // Is the adc is about to start a new conversion batch?
      {
         smProcessTick();
      }
      else if ( adcHasNewValue() ) // Do we have a decimated value to process?
      {
         processAdcValue();
      }

      dbgClear(DBG_MAIN_LOOP);
   }

   // Unreachable !
   return 0;
}


/* ----------------------------  End of file  ---------------------------- */
