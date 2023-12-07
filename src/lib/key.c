/**
 *@ingroup lib
 *@defgroup key Single Key API
 *@{
 *@file
 *****************************************************************************
 * Manages a single key or push button.
 * To use this API, first initialise it calling keyInit. A couple of callbacks
 *  are required. These are called when a key press is detected.
 * Also, the function keyScan must be called periodically, ideally every 20ms
 *  (50Hz) or thereabout. 
 * The parameters KEY_DEBOUNCE_CYCLES and KEY_LONG_CYCLES can be adjusted 
 *  in config.h to modify the responsivity of the keypad. They are set for a
 *  50Hz sampling rate at 3 and 40 respectively.
 * The function keyScan can be called within an interrupt, but the callback
 *  method would also be called with the same interrupt.
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

#include "wgx.h"
#include "key.h"

// Set the default KEY_DEBOUNCE_CYCLES and KEY_LONG_CYCLES
// This assumes a 50Hz sampling rate
#if !defined KEY_DEBOUNCE_CYCLES
   #define KEY_DEBOUNCE_CYCLES 3
#endif

#if !defined KEY_LONG_CYCLES
   #define KEY_LONG_CYCLES 40
#endif

#if !defined KEY_PORT || !defined KEY_BIT
   #error "The key API requires the KEY_PORT and KEY_BIT resources to be defined"
#endif

// Reassign macro defined constants in typed variables

/** Number of samples to debounce a key */
static const uint8_t keyDebounceCycles = KEY_DEBOUNCE_CYCLES;

/** Number of sampling cycles for a long push to be detected */
static const uint8_t keyLongCycles = KEY_LONG_CYCLES;

/** Defines different stages of the keypad state machine */
typedef enum
{
   /** No key activity */
   keyIdle_e = 0,
   /** Single, breif key press */
   keySingle_e,
   /** Long key press */
   keyLong_e
} keyState_t;


/** Keep the current state of the key */
static keyState_t state;

/** Count how many cycles the key was down */
static uint8_t consecutiveDowns;

/** Function to call when a quick push is detected */
static keyPushedCallback_t keyShortPushCallBack;

/** Function to call when a long push is detected */
static keyPushedCallback_t keyLongPushCallBack;


/**
 * Initialise the keypad and its ste machine
 */
void keyInit(
   keyPushedCallback_t shortPushCallBack, 
   keyPushedCallback_t longPushCallBack )
{
   // Store the callbacks
   keyShortPushCallBack = shortPushCallBack;
   keyLongPushCallBack = longPushCallBack;

   // Activate the pull-up on the port pin
   KEY_PORT |= _BV(KEY_BIT);

   // Configure as input
   DDR_OF(KEY_PORT) &= ~_BV(KEY_BIT);

   // Reset key state
   state = keyIdle_e;
   consecutiveDowns = 0;
}

/**
 * Scan the keypad for changes.
 * It is up to the application to call this method periodically
 *  every 20ms or thereabout.
 */
void keyScan()
{
   // Scan the key
   if ( ( PIN_OF(KEY_PORT) & _BV(KEY_BIT)) == 0 )
   {
      // Key is down
      switch ( state )
      {
      case keyIdle_e:
         if ( ++consecutiveDowns > keyDebounceCycles )
         {
            state = keySingle_e;
         }
         break;
      case keySingle_e:
         if ( ++consecutiveDowns > keyLongCycles )
         {
            // Change state and immediatly notify the state machine of a long key push
            state = keyLong_e;
            keyLongPushCallBack();
         }
         break;
      case keyLong_e:
         // Stay there until the key is released
         break;
      }
   }
   else
   {
      switch ( state )
      {
      case keyIdle_e:
         consecutiveDowns = 0;
         break;
      case keySingle_e:
         // If the switch was pushed more than the debounce (but less than a long push),
         //  validate the push
         if ( consecutiveDowns > keyDebounceCycles )
         {
            // Change state and immediatly notify the state machine of a long key push
            state = keyIdle_e;
            keyShortPushCallBack();
         }
         consecutiveDowns = 0;
         break;
      case keyLong_e:
         state = keyIdle_e;
         consecutiveDowns = 0;
         break;
      }
   }
}


/* ----------------------------  End of file  ---------------------------- */
