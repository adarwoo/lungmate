#ifndef __CONFIG_H_HAS_ALREADY_BEEN_INCLUDED__
#define __CONFIG_H_HAS_ALREADY_BEEN_INCLUDED__
/**
 *@ingroup lungmate
 *@{
 *@file
 *****************************************************************************
 * Holds all the configuration for the entire application.
 * This includes port definition, timings, connections, threshold values etc.
 * This file is always including by the wgx.h header. Therefore, do not rename it,
 *  and include wgx.h instead.
 * In fact, never include this file directly.
 *
 * @author software@arreckx.com
 *****************************************************************************
 */


/**
 * Code revision to be updated manually or through build and
 * displayed in the log upon start-up
 */
#define REVISION       "1.3"

/*
 *  MPU internals
 */

/** Internal main clock */
#define SYS_CLOCK 8192000


// ---------------------------------------------------------------------------
// Pins used for debugging
// ---------------------------------------------------------------------------

/** Resource used for the debug pin 0 */
#define DBG_0_PORT     PORTB
#define DBG_0_BIT      0

/** Resource used for the debug pin 1 */
#define DBG_1_PORT     PORTB
#define DBG_1_BIT      1

/** Resource used for the debug pin 2 */
#define DBG_2_PORT     PORTB
#define DBG_2_BIT      2

/** Unused pin */
#define DBG_3_PORT     PORTA
#define DBG_3_BIT      2

/** Unused pin */
#define DBG_4_PORT     PORTA
#define DBG_4_BIT      4

// Define the function for each debug pins
#define DBG_ADC        0
#define DBG_UART       1
#define DBG_MAIN_LOOP  2


// ---------------------------------------------------------------------------
// Other configurations
// ---------------------------------------------------------------------------

/** Mode LED */
#define MODE_LED_PORT  PORTB
#define MODE_LED_BIT   6

/** Relay connection */
#define RELAY_PORT     PORTA
#define RELAY_BIT      6


// ---------------------------------------------------------------------------
// Key configuration - required by the key module
// ---------------------------------------------------------------------------

/** Unique key (external) */
#define KEY_PORT       PORTA
#define KEY_BIT        7


// ---------------------------------------------------------------------------
// Uart configurations - required by the uart module
// ---------------------------------------------------------------------------

/** Common baud speed for the serial Tx */
#define UART_BAUD      19200

/** Pin to use to tramsmit serial data */
#define UART_TX_PORT   PORTA
#define UART_TX_BIT    5

/** Pin to use to receive serial data */
#define UART_RX_PORT   PORTB
#define UART_RX_BIT    3


// ---------------------------------------------------------------------------
// ADC configuration - required by the adc module
// ---------------------------------------------------------------------------

/**
*  Mux setup for the ADC. Set AVR documentation, ADC section.
* Set the mux as ADC0+ ADC1- (with signal on + and the ref on -)
*/
#define ADC_MUX 0b100001

/** Sampling frequency (after decimation if used) */
#define ADC_SAMPLE_FREQUENCY 320


// ---------------------------------------------------------------------------
// FFT configuration - required by the fft module
// ---------------------------------------------------------------------------

/** Size of the fft in 2^n */
#define FFT_M 6


/*-
 *  Configure the state machine
 */

/**
 * How many consecutive above the threshold FFT cycle (.2s)
 *  towait before turning the relay ON
 */
#define SM_COUNT_ON_ACTIVATE 5

/** Same but off */
#define SM_DEFAULT_COUNT_OFF_DEACTIVATE 20

/** The triggering threshold in watt */
#define SM_DEFAULT_THRESHOLD 50

/** Flashing periode in 320Hz cycles */
#define SM_MODE_BLINK_PERIOD 8


// ---------------------------------------------------------------------------
// nvParam configuration relative to src/
// ---------------------------------------------------------------------------
#define NV_PARAM_DEFS_FILENAME "lungmate/nvParamDefs.h"

#endif // ndef __CONFIG_H_HAS_ALREADY_BEEN_INCLUDED__

