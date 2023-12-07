/**
 *@ingroup lib
 *@defgroup sim PC Simulation
 *@{
 *@file
 *****************************************************************************
 * Simulates poorly the adc. Creates simple stub to allow application using
 *  the adc converter to compile under the simulation
 *
 * @author software@arreckx.com
 *****************************************************************************
 */
#include "adc.h"

volatile bool adcNewValueFlag;
volatile bool adcNewConversionStartedFlag;
volatile int16_t adcValue;

/** Empty stub. Does nothing */
void adcInit()
{
}

/** Empty stub. Does nothing */
void adcShutdown()
{
}


/* -----------------------------  End of file  ---------------------------- */

