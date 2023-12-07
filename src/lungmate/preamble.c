/**
 *@ingroup lungmate
 *@{
 *@file
 *****************************************************************************
 * Implements all methods required by the applications and called prior to the
 *  core application starting.
 * Some of the methods defined (preamblePaintStack, preambleGetStatusRegister)
 *  are called automatically before main.
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

#include "wgx.h"
#include "uart.h"
#include "nvParamDefs.h"
#include "fft.h"
#include "uart.h"
#include "dbg.h"
#include "stateMachine.h"
#include "key.h"
#include "adc.h"


//
// 1 - Paint the stack to detect overflows
//

/**
 * Called even before main to fill the stack space with a fixed
 *  pattern to make looking for overflow or available stack space easier.
 * The method must be in assembler since it touches the stack space
 */
void preamblePaintStack(void)
   __attribute__ ((naked))
   __attribute__ ((section (".init1")));

void preamblePaintStack()
{
#ifdef C_CODE_NOT_TO_BE_USED_SINCE_WE_HAVE_NO_STACK
   uint8_t *p = &_end;

   while(p <= &__stack)
   {
      *p = 0xc5; /* Pattern is c5 - 11000101 */
      p++;
   }
#else  // Assembler equivalent
   __asm volatile (
	"    ldi r30,lo8(_end)    \r\n"
   "    ldi r31,hi8(_end)    \r\n"
   "    ldi r24,lo8(0xc5)    \r\n"
   "    ldi r25,hi8(__stack) \r\n"
   "    rjmp .cmp            \r\n"
   ".loop:                   \r\n"
   "    st Z+,r24            \r\n"
   ".cmp:                    \r\n"
   "    cpi r30,lo8(__stack) \r\n"
   "    cpc r31,r25          \r\n"
   "    brlo .loop          	\r\n"
   "    breq .loop"::);
#endif
}

//
// 2 - Early on, disable the watchdog and copy the reason for the reset
//

/** Keep a copy of the MPU status register */
uint8_t preambleStatusRegisterMirror
   __attribute__ ((section (".noinit")));

void preambleGetStatusRegister(void)
   __attribute__((naked)) \
   __attribute__((section(".init3")));

/**
 *  This method is called early on (section init3) and makes a
 *  copy of the status register and sets the watchdog to the
 *  longest period to give enough time to the initialisations
 *  to complete
 */
void preambleGetStatusRegister()
{
   preambleStatusRegisterMirror = MCUSR; MCUSR = 0;
#ifdef NDEBUG
   // The watchdog is always on, we set it to its longest reset to
   //  give plenty of time at startup, and avoid repeated reset and case
   //  of dramatic failure.
   wdt_enable( WDTO_8S );
#endif
}


//
// 3 - Override the exit method to get exit diag
//

/**
 * Overrides the stdlib exit method to provide extra diagnostic
 *
 * @param why   An integer providing extra information as to reason of exit
 */
void exit( int why )
{
   cli();
   uartLogError("Exit :");
   uartPrintNumber(why,0);

   // Force a watchdog reset or lock if debugging
   for (;;)
   {
      continue;
   }
}


/**
 * Initialise all the APIs used for this project
 *  and display diagnostic information
 * This method should be the first to be called.
 */
void preamble(void)
{
   // Shutdown the USI to help reduce the digital noise
   power_usi_disable() ;

   // Initialise the debug ports
   dbgInit();

   // Initialise the serial comms. The shutdown callback is safe to call even
   //  before the ADC has been initialised
   uartInit( adcShutdown );

   // Welcome message out on the uart
   uartLogInfo( "Lungmate v" REVISION );

   // Display reason for reset
   if ( bit_is_set(preambleStatusRegisterMirror, WDRF) )
   {
      uartLogInfo( "watchdog reset" );
   }

   if ( bit_is_set(preambleStatusRegisterMirror, BORF) )
   {
      uartLogInfo( "brownout reset" );
   }

   if ( bit_is_set(preambleStatusRegisterMirror, EXTRF) )
   {
      uartLogInfo( "external reset" );
   }

   if ( bit_is_set(preambleStatusRegisterMirror, PORF) )
   {
      uartLogInfo( "power on reset" );
   }

   // Initialise the parameter storage in eeprom
   nvParamInit();

   // Initialise the fft
   fftInit( nvParam(centerFrequency_e) );

   // Initialise the single key. The callbacks update the statemachine
   keyInit( smProcessShortKey, smProcessLongKey );

   // Initialise the state machine
   smInit();

   // Activate the watchdog
   // For release versions, the watch dog should be on all the time.
   // When debugging, keep it off to allow breakpoints
   #ifdef NDEBUG
      // An fft result is computed every 200ms, which will reset the watchdog
      // The wd is set just well over twice this delay to avoid race conditions.
      wdt_enable( WDTO_1S );
   #endif // NDEBUG

   // Initialise the ADC, which sets the timer and associated interrupts
   adcInit();

   // Set the sleep mode to idle - (ADC save is not an option as we need the timers)
   set_sleep_mode(0);
}


/* ----------------------------  End of file  ---------------------------- */

