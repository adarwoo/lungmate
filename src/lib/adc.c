/**
 *@ingroup lib
 *@defgroup adc Analog Conversion API
 *@{
 *@file
 *****************************************************************************
 * Manages the on-board ADC converter and implements the ADC API.
 * This API, once initialise, will make burst of 16 conversions at 320Hz
 *  using the timer1 to generate the main sampling rate and the ADC 
 *  conversion finished interrup to manage the burst.
 * The first conversion is started by entering sleep. The sleep mode
 *  cannot be ADC Save since we need the timer1 running.
 * The caller can check whether a new value is ready by calling adcHasNewValue.
 * There should be no other running interrupts whilst a conversion is taking
 *  place to limit the noise.
 * Each new conversion cycle will inverted the debug pin. Each end of 
 *  conversion interrupt will revert the pin polarity so a blip can be seen.
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

#include "wgx.h"
#include "adc.h"
#include "dbg.h"

/** Using the clock, workout the level of pre-scaling required */
#define ADC_RAW_ADC_COMPARE_VALUE (SYS_CLOCK/ADC_SAMPLE_FREQUENCY)

/**
 * Work out the optimum prescaler value such that the output
 * compare value fits within 10-bits. This should limit the
 * error to a minimum. Values are for clock up to 20MHz (max
 * allowed on the AVR Tiny).
 */ 
#if ADC_RAW_ADC_COMPARE_VALUE < 1024
   /** Actual value of the prescaler */
   #define ADC_PRESCALE_VALUE 1
   /** Mask to supply to set the prescale value chosen */
   #define ADC_PRESCALE_MSK   _BV(CS10)
#elif ADC_RAW_ADC_COMPARE_VALUE < 2048
   #define ADC_PRESCALE_VALUE 2
   #define ADC_PRESCALE_MSK   _BV(CS11)
#elif ADC_RAW_ADC_COMPARE_VALUE < 4096
   #define ADC_PRESCALE_VALUE 4
   #define ADC_PRESCALE_MSK   _BV(CS11) | _BV(CS10)
#elif ADC_RAW_ADC_COMPARE_VALUE < 8192
   #define ADC_PRESCALE_VALUE 8
   #define ADC_PRESCALE_MSK   _BV(CS12)
#elif ADC_RAW_ADC_COMPARE_VALUE < 16384
   #define ADC_PRESCALE_VALUE 16
   #define ADC_PRESCALE_MSK   _BV(CS12) | _BV(CS10)
#elif ADC_RAW_ADC_COMPARE_VALUE < 32768
   #define ADC_PRESCALE_VALUE 32
   #define ADC_PRESCALE_MSK   _BV(CS12) | _BV(CS11)
#elif SYS_CLOCK <= 20000000
   #define ADC_PRESCALE_VALUE 64
   #define ADC_PRESCALE_MSK   _BV(CS12) | _BV(CS11) | _BV(CS10)
#else
   #error "System clock > 20MHz are not supported"
#endif

/** Work out the output compare value (Top) */
static const uint16_t ADC_COMPARE_VALUE = (SYS_CLOCK/(ADC_SAMPLE_FREQUENCY*ADC_PRESCALE_VALUE))-1;

/** Stores the decimated value of the ADC result */
static volatile int16_t decimated;

/** Counter for the decimation */
static volatile uint8_t decimationCounter;

/*-
 * To pass values to the main application
 */

/** true to indicates a new value is ready. The application must the clear it */
volatile bool adcNewValueFlag;

/** 
 * true to indicate a new conversion has started
 * cleared when calling adcHasNewConversionStarted 
 */
volatile bool adcNewConversionStartedFlag;

/** Holds the final decimated value - on 12bits now */
volatile int16_t adcValue;


/**
 * Called to setup the ADC
 * The ADC is triggered by a sleep
 */
void adcInit()
{
   // Init the flags
   adcNewValueFlag = false;
   adcNewConversionStartedFlag = false;

   // Setup the interrupt
   decimationCounter=0;

   //
   // Set the ADC registers
   //

   // Use internal 2.56V internal reference, decoupled externally
   // Set the mux as ADC5+ ADC4- (with signal on + and the ref on -)
   ADMUX = _BV(REFS1) | _BV(REFS0) | (ADC_MUX & 0b11111);

   // Set the A/D with interrupt enable and prescale by 64
   ADCSRA = _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1);

   // Set bipolar mode, gain to 8, 2.56V ref (completes setup of ADMUX)
   ADCSRB = _BV(BIN) | _BV(GSEL) | _BV(REFS2) | _BV(MUX5 * (ADC_MUX>>5));

   // Turn off digital input all all unused digital pins
   DIDR0 = _BV(ADC5D) | _BV(ADC4D) | _BV(AREFD);

   //
   // Set timer1 which gives the beat for the ADC conversions
   //

   // Set the compare value (10-bits mode) for the OCR1C which controls the TOP
   //  value of the timer. When TOP is reached, a TOV interrupt is triggered.
   TC1H = ADC_COMPARE_VALUE >> 8;
   OCR1C = ADC_COMPARE_VALUE & 0xff;

   // Enable the timer overflow interrupt
   TIMSK |= _BV(TOIE1);

   // Set the clock source and reset the prescaler
   TCCR1B = _BV(PSR1) | ADC_PRESCALE_MSK;

   // Enable interrupts
   sei();
}


/**
 * Shutdown the ADC and the tick timer. This should be called from within an interrupt
 *  to prevent any race conditions.
 * This should allow other part of the system to run
 * Call adcInit to resume operations
 */
void adcShutdown()
{
   // Turn off Timer0
   TCCR1B = _BV(PSR1); // Clock stopped and reset prescale

   // Turn off the ADC
   ADCSRA &= ~_BV(ADEN);

   // Disable time interrupt and clear pending
   TIMSK &= ~_BV(TOIE1);
   TIFR = _BV(TOV1);

   // Disable ADC interrupt and clear pending
   ADCSRA &= ~_BV(ADIE);
   ADCSRA = _BV(ADIF);
}


/**
 * Interrupt handler called periodically, every 320Hz to start a
 * burst of ADC convertions.
 * All this interrupt does is to turn the ADC on again. It does
 * not start a new conversion, as this will automatically happen
 * when we enter sleep mode
 *
 * This interrupt is also used to scan the keppad, and update
 * the state machine accordingly. Although this could introduce
 * some jitter in the conversion process, it should have a
 * negligable effect.
 */
ISR( TIMER1_OVF_vect )
{
   dbgToggle(DBG_ADC);

   // Turn-on the A/D converter, which will automatically start once we enter sleep mode
   ADCSRA |=_BV(ADEN);

   // Flag a new conversion was started
   adcNewConversionStartedFlag = true;
}


/**
 * Interrupt called upon a conversion finished. 
 * The ADC value is read, decimated and the adcNewValueFlag set 
 *  if necessary.
 */
ISR(ADC_vect)
{
   int8_t adcl;
   int8_t adch;

   // Toggle the debug led (this will show a blip in the sequence)
   dbgToggle(DBG_ADC);

   // Read the value. It is given as two's complement spreading as 10bits over 2 registers
   adcl = ADCL;  // read out ADCL register and lock ADCH
   adch = ADCH;  // read out ADCH register

   // Since the ADC is configured in biploar mode, the result needs to be adjusted
   // If the MSB is 1, the number is negative and needs adjusting
   if ( adch & 0b10 )
   {
      // Adjust 2's complement over 16 bits
      adch |= 0xFE;
   }

   // Accumulate over the decimated value
   decimated += (adch<<8 | adcl);

   // Increment the decimation counter
   ++decimationCounter;

   // Fast modulo 16
   decimationCounter &= 0x0F;

   // Have we collected enough values to decimate?
   if ( decimationCounter == 0 )
   {
      // Adjust the value to remove the noise
      adcValue = decimated >> 2;

      // Set the flag to be read by the main loop
      adcNewValueFlag = true;

      // Reset the decimation
      decimated = 0;

      // Turn off the ADC, so that no new conversion will be started upon entering
      // sleep
      ADCSRA &= ~_BV(ADEN);
   }

   // Toggle the debug LED back
   dbgToggle(DBG_ADC);
}


/* ----------------------------  End of file  ---------------------------- */

