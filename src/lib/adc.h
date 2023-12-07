#ifndef __ADC_H__HAS_ALREADY_BEEN_INCLUDED__
#define __ADC_H__HAS_ALREADY_BEEN_INCLUDED__
/**
 *@ingroup adc
 *@{
 *@file
 *****************************************************************************
 * Defines the public API for the AD conversion.
 * This A/D API makes an unrestricted use of the timer 0 of the AVR.
 * The library must first be initialise with adcInit.
 * For operations not requiring A/D conversion, the ADC converted and timer 0 should
 *  be shutdown using adcShutdown.
 * Once initialised, regular conversions will take place in the background, managed solely
 *  by interrupts. It is important that the main code enters sleep has soon and often as possible
 *  possibly only doing that, and check if new decimated values are ready by calling
 *  adcHasNewValue. 
 * Once the value (obtained with adcGetValue) has been processed, it must
 *  be reset to prepare for the next incomming value using adcValueProcessed.
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

#include "wgx.h"

extern volatile bool adcNewValueFlag;
extern volatile bool adcNewConversionStartedFlag;
extern volatile int16_t adcValue;

void adcInit(void);
void adcShutdown(void);

/**
 * Reports wether a new results is ready.
 * Calling this method resets the internal flag.
 *
 * @return true is a new result is ready
 */
static inline bool adcHasNewValue(void)
{
   bool retval;

   // Force atomic read and reset
   cli();
   retval = adcNewValueFlag;
   adcNewValueFlag = false;
   sei();

   return retval;
}

/**
 * Return the sampled, decimated and signed adjusted value.
 * The value has been adjusted to reflect the over-sampling and decimation
 *  and is provided as a regular 16 bits signed number.
 * No other adjusted is made, so the result is the raw value has measured
 *  by the adc converter.
 *
 * @return The sampled value in a signed 16 bits number.
 */
static inline int16_t adcGetValue(void)
   { return adcValue; }

/**
 * Check whether a new conversion is in progress.
 * This call will return true at every start of conversion.
 * Since the converted value is decimated, more adc conversions
 *  will take place afterwards.
 * Calling this function reset the flag.
 *
 * @return true if a new conversion is started.
 */
static inline bool adcTick(void)
{
   bool retval;

   // Force atomic read and reset
   cli();
   retval = adcNewConversionStartedFlag;
   adcNewConversionStartedFlag = false;
   sei();

   return retval;
}

#endif   /* ndef __ADC_H__HAS_ALREADY_BEEN_INCLUDED__ */
