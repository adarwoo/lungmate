/**
 *@ingroup lungmate
 *@{
 *@file
 *****************************************************************************
 * State machine controlling the action of the keypad and the state of
 *  the relay.
 * This state machine is also responsible for turning all LEDs on/off
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

#include "wgx.h"
#include "nvParam.h"
#include "stateMachine.h"
#include "key.h"
#include "fft.h"
#include "adc.h"
#include "console.h"
#include "uart.h"

/** 
 * Size of the hysteris for the detection in Watt. This value 
 * means that a fluctation less than that size would not cause 
 * another detection 
 **/ 
static const uint8_t THRESHOLD_HYSTERISIS = 20;

/** Defines the required states for the relay */
typedef enum
{
   /** Relay is closed */
   smRelayOn_e,
   /** Relay is open */
   smRelayOff_e,
} smStatus_t;

/** Defines the current mode of operation */
typedef enum
{
   /** DE is off */
   smModeOff_e,
   /** DE is on */
   smModeOn_e,
   /** Auto mode for DE */
   smModeAuto_e
} smMode_t;

/** System timer tick of the time when it was off */
static uint32_t timeStamp;

/** Consecutive above the threshold */
static uint8_t countOn;

/** Consecutive below the threshold */
static uint8_t countOff;

/** Current status */
static smStatus_t status;

/** Current mode */
static smMode_t mode;

/** For the blinking */
static uint8_t blinkCycle;

/** Trigger off thresholds in Watts */
static uint16_t thresholdLow;

/** Trigger on thresholds in Watts */
static uint16_t thresholdHigh;

/** Keep relay on n * 200ms after the load has dropped */
static uint8_t countOffDeactivate;


/*-
 * Helpers for controlling the ports
 */

/** Turns the relay OFF. (Open the relay) */
static inline void relayOff(void)
   { RELAY_PORT &= ~_BV(RELAY_BIT); }

/** Turns the relay ON. (Close the relay) */
static inline void relayOn(void)
   { RELAY_PORT |= _BV(RELAY_BIT); }

/** Turn on both mode LEDs */
static inline void modeLedOn(void)
{
   MODE_LED_PORT |= _BV(MODE_LED_BIT);
}

/** Turn off both mode LEDs */
static inline void modeLedOff(void)
{
   MODE_LED_PORT &= ~ _BV(MODE_LED_BIT);
}

/** Alternate both mode LEDs */
static inline void modeLedToggle(void)
{
   MODE_LED_PORT ^= _BV(MODE_LED_BIT);
}


/**
 * Update the front LED to indicate the mode
 * This method is called just before the keypad, at regular
 * interval of 20ms.
 *
 * OFF  -> LED off
 * ON   -> LED on
 * AUTO -> LED flashing
 */
static void smUpdateModeLed(void)
{
   switch ( mode )
   {
   case smModeOff_e:
      // Turn off the LED
      modeLedOff();
      blinkCycle = 0;
      break;
   case smModeOn_e:
      // Turn ON the LED
      modeLedOn();
      blinkCycle = 0;
      break;
   case smModeAuto_e:
      // Flash fast - 4Hz, T=250ms, Half T=125ms
      // Alternate LED every 6 cycles
      if ( blinkCycle == 0 )
      {
         // New cycle - simply flip bit. That way, if on, it goes off, off it goes on
         modeLedToggle();
      }

      if ( ++blinkCycle > SM_MODE_BLINK_PERIOD )
      {
         blinkCycle = 0;
      }
      break;
   }
}


/**
 * Initialise the state machine and the front panel
 */
void smInit(void)
{
   // Prepare the relay port
   relayOff();
   DDR_OF(RELAY_PORT)  |= _BV(RELAY_BIT);

   // Prepare the mode indication LED
   modeLedOff();
   DDR_OF(MODE_LED_PORT) |= _BV(MODE_LED_BIT);

   // Initialise the mode from eeprom value
   if ( nvParam(restartInAutoMode_e) == 1 )
   {
      mode = smModeAuto_e;
   } 
   else
   {
      mode = smModeOff_e;
   }

   // Initialise the state machine internals
   status = smRelayOff_e;
   timeStamp = 0;
   countOn = 0;
   countOff = 0;
   blinkCycle = 0;

   // Triggering threshold given in Watts
   thresholdHigh = (uint16_t)nvParam(powerThreshold_e);

   // Compute some hysterisis to avoid excessive on/off cycles
   thresholdLow = thresholdHigh - THRESHOLD_HYSTERISIS;

   // Time to keep the relay on after the load has come off, expressed as 1/5th of seconds
   countOffDeactivate = (uint8_t)nvParam(keepOnAfter_e) * 5;
}


/**
 * Process a new value computed by the FFT
 *
 * @param value Measured power in Watt
 */
void smProcessFFTResult( uint16_t value )
{
   // Determine whether we should be on or off
   //  independently of the state we're in (on/off/auto)
   // This is to allow the system to come on instantanously when
   //  swithing between modes, and to avoid turning off when alternating
   //  between ON and AUTO.
   if ( status == smRelayOff_e && value >= thresholdHigh )
   {
      countOff = 0;

      if ( ++countOn > SM_COUNT_ON_ACTIVATE )
      {
         status = smRelayOn_e;
      }
   }

   if ( status == smRelayOn_e && value <= thresholdLow )
   {
      countOn = 0;

      if ( ++countOff > countOffDeactivate )
      {
         status = smRelayOff_e;
      }
   }

   // Given the mode we're in, toggle the relay.
   switch ( mode )
   {
   case smModeOn_e:
      relayOn();
      break;
   case smModeAuto_e:
      if ( status == smRelayOn_e )
      {
         relayOn();
      }
      else
      {
         relayOff();
      }
      break;
   default:
      relayOff();
   }
}


/**
 * Called by the key pad to indicate a single push was detected
 * Single push cycles the modes:
 *  OFF -> AUTO -> ON -> AUTO ...
 */
void smProcessLongKey(void)
{
   switch ( mode )
   {
   case smModeOff_e:
      mode = smModeAuto_e;
      break;
   case smModeOn_e:
      mode = smModeAuto_e;
      break;
   case smModeAuto_e:
      mode = smModeOn_e;
      break;
   }
}


/**
 * Called by the keypad to indicate a long push was detected. A
 * long push act to set the mode from its current. The mode
 * switches as follow:

 *  AUTO -> OFF  -> ON -> OFF
 */
void smProcessShortKey(void)
{
   switch ( mode )
   {
   case smModeOff_e:
      mode = smModeOn_e;
      break;
   case smModeOn_e:
      mode = smModeOff_e;
      break;
   case smModeAuto_e:
      mode = smModeOff_e;
      break;
   }
}


/**
 * A new conversion had been started. Use this event to sample the
 *  keypad and update the state machine.
 */
void smProcessTick(void)
{
   // Prescaler counter used to create a secondary timer for the keypad handling
   static uint8_t counter = 0;

   // Create a prescaler top value from the adc sample frequency (the primary tick)
   //  and the key prefered sampling rate.
   static const uint8_t tickPrescalerTop =
      ADC_SAMPLE_FREQUENCY / KEY_PREFERED_SCANNING_RATE;

   // Check whether we need to scan the keypad
   // Prescale te main adc clock to obtain a slower tick suitable to sample the keypad
   if ( ++counter > tickPrescalerTop )
   {
      // Reset the counter (bottom value)
      counter = 0;

      // Sample the key
      keyScan();

      // The mode LED may flash in some cases - use the keypad timing (50Hz) to also control this LED
      // This is much better than relying on a different interrupt which could generate noise during
      //  the ADC convertion.
      // Calling this method right after the keypad (which updates the SM)
      //  guarantees the LED is updated in real time with changes introduces by the keypad
      smUpdateModeLed();
   }
}


/* ----------------------------  End of file  ---------------------------- */
