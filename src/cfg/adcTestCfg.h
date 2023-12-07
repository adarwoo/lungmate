/**
 *@ingroup adc
 *@defgroup adc_test Unit test
 *@{
 *@file
 *****************************************************************************
 * Configuration for the ADC unit test.
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

// Use the default config for most parameters
#include "cfg.h"

//
// Override the adc section for the test
//

/** 
 *  Mux setup for the ADC. Set AVR documentation, ADC section.
 * Set the mux as ADC5+ ADC4- (with signal on + and the ref on -)
 */ 
#undef ADC_MUX
#define ADC_MUX 0b101101

/** Sampling frequency (after decimation if used) */
#undef ADC_SAMPLE_FREQUENCY 
#define ADC_SAMPLE_FREQUENCY 40


/* ----------------------------  End of file  ---------------------------- */

