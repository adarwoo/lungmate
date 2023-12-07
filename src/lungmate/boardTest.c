/**
 *@ingroup lungmate
 *@defgroup board_test Unit test for the board
 *@{
 *@file
 *****************************************************************************
 * Entry point for testing the board's electronics.
 * This application is pretty close to the full program but focuses on
 *  validating the I/Os.
 * It should only be executed once all the API unit tests have passed as it
 *  uses all of it.
 * The A/D converter is used to give the tick. Pressing the key will change
 *  the LED status. Long key press will change the state of the relay.
 * Characters received on the uart are echoed on a single line.
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

#include "wgx.h"
#include "uart.h"
#include "dbg.h"
#include "adc.h"
#include "key.h"

// Reuse the stateMachine helper - helps testing the stateMachine too
#include "stateMachine.c"


static const uint8_t LED_PRESCALE = 25;

/** Flash the board mode LED */
void _updateLed(void)
{
   // Pre-scale counter from the key scan tick (20ms)
   static uint8_t ledPrescale = 0;

   if ( ++ledPrescale > LED_PRESCALE )
   {
      ledPrescale=0;

      modeLedToggle();
   }
}


/** Switch on relay. Should turn on the status LED */
void _switchRelay(void)
{
   static bool relayIsOn = false;

   if ( relayIsOn )
   {
      relayOff();
      relayIsOn = false;
   }
   else
   {
      relayOn();
      relayIsOn = true;
   }
}


/** 
 * Process incomming traffic from the serial port
 * Simply echo the character back
 */
static inline void _processRx(void)
{
   // Read the character -  will block until the char has been fully received
   // This will clear the uartRxActive internal flag
   int c = uartGetChar();

   // Echo back the character
   uartSendChar(c);
   uartSendChar('\n');

   // Resume measurements and re-enable the interrupts that were
   //  disabled when a new character was received through a callback
   adcInit();
}


/**
 * A new conversion had been started. 
 * Use this event to scan the key pad and update the LEDs
 */
static inline void _onTick(void)
{
   // Prescaler counter used to create a secondary timer for the keypad handling
   static uint8_t counter = 0;

   // Create a prescaler top value from the adc sample frequency (the primary tick)
   //  and the key prefered sampling rate.
   static const uint8_t tickPrescalerTop =
      ADC_SAMPLE_FREQUENCY / KEY_PREFERED_SCANNING_RATE;

   // Check whether we need to scan the keypad
   // Prescale te main adc tick to obtain a slower tick suitable to sample the keypad
   if ( ++counter > tickPrescalerTop )
   {
      // Reset the counter (bottom value)
      counter = 0;

      // Sample the key
      keyScan();

      _updateLed();
   }
}


/**
 * Entry point of this application.
 * Initialises all peripherals through the supplied API and
 *  finish initialising non used PORT pins.
 * Also, turn off non essential peripheral to reduce the digital
 *  noise.
 *
 * @return 0 for ANSI compiance only. Should never ever return.
 */
int main(void)
{
   // Initialise the debug ports
   dbgInit();

   // Initialise the serial comms. The shutdown callback is safe to call even
   //  before the ADC has been initialised
   uartInit( adcShutdown );

   // Welcome message out on the uart
   uartLogInfo( "Test Board v" REVISION );

   // Initialise the parameter storage in eeprom
   nvParamInit();

   // Initialise the single key. The callbacks update the statemachine
   keyInit( modeLedToggle, _switchRelay );

   // Initialise the state machine - in fact, just the IO for LEDs and relay
   smInit();

   // Initialise the ADC, which sets the timer and associated interrupts
   // This also will start an ADC conversion as soon as we enter a sleep
   adcInit();

   // Set the sleep mode to idle - (ADC save is not an option as we need the timers)
   set_sleep_mode(0);

   // Enter the main loop where we wait for the decimation to have completed
   for (;;)
   {
      sleep_mode();

      dbgSet(DBG_MAIN_LOOP);

      if ( uartHasChar() ) // Is the serial port receiving a new character?
      {
         _processRx();
      }
      else if ( adcTick() )
      {
         _onTick();
      }

      dbgClear(DBG_MAIN_LOOP);
   }

   // Unreachable !
   return 0;
}


/* ----------------------------  End of file  ---------------------------- */
