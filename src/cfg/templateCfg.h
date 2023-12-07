#ifndef __CONFIG_H_HAS_ALREADY_BEEN_INCLUDED__
#define __CONFIG_H_HAS_ALREADY_BEEN_INCLUDED__
/**@file
 *****************************************************************************
 * Template config file for projectX
 * 'n' should be replaced by an integer from 0 to 7.
 * PORTx should be replaced by a valid port name, like PORTA
 *
 * @author software@arreckx.com
 *****************************************************************************
 */


// ---------------------------------------------------------------------------
// General settings
// ---------------------------------------------------------------------------

/** Required by console and unit tests. Displayed in the log upon start-up */
//#define REVISION       "x.x"

/** Internal main clock in Hz */
//#define SYS_CLOCK 8000000


// ---------------------------------------------------------------------------
// Pins used for debugging
// ---------------------------------------------------------------------------

/** Resource used for the debug pin 0 */
//#define DBG_0_PORT     PORTx
//#define DBG_0_BIT      n

/** Resource used for the debug pin 1 */
//#define DBG_1_PORT     PORTx
//#define DBG_1_BIT      n

/** Resource used for the debug pin 2 */
//#define DBG_2_PORT     PORTx
//#define DBG_2_BIT      n

/** Resource used for the debug pin 3 */
//#define DBG_2_PORT     PORTx
//#define DBG_2_BIT      n

/** Resource used for the debug pin 4 */
//#define DBG_2_PORT     PORTx
//#define DBG_2_BIT      n

/** Resource used for the debug pin 5 */
//#define DBG_2_PORT     PORTx
//#define DBG_2_BIT      n

/** Resource used for the debug pin 6 */
//#define DBG_2_PORT     PORTx
//#define DBG_2_BIT      n

/** Resource used for the debug pin 7 */
//#define DBG_2_PORT     PORTx
//#define DBG_2_BIT      n

// Define the function for each debug pins. Use meaning full names
//#define DBG_ADC        n
//#define DBG_UART       n
//#define DBG_XXX2       n
//#define DBG_XXX3       n
//#define DBG_XXX4       n
//#define DBG_XXX5       n
//#define DBG_XXX6       n
//#define DBG_XXX7       n


// ---------------------------------------------------------------------------
// Key configuration - required by the key module
// ---------------------------------------------------------------------------

/** Unique key (external) */
//#define KEY_PORT       PORTx
//#define KEY_BIT        n


// ---------------------------------------------------------------------------
// Uart configurations - required by the uart module
// ---------------------------------------------------------------------------

/** Common baud speed for the serial Tx */
//#define UART_BAUD      19200

/** Pin to use to tramsmit serial data */
//#define UART_TX_PORT   PORTx
//#define UART_TX_BIT    n

/** Pin to use to receive serial data */
//#define UART_RX_PORT   PORTx
//#define UART_RX_BIT    n


// ---------------------------------------------------------------------------
// ADC configuration - required by the adc module
// ---------------------------------------------------------------------------

/** 
*  Mux setup for the ADC. Set AVR documentation, ADC section.
*  Example, set the mux as ADC5+ ADC4- (with signal on + and the ref on -)
*/ 
//#define ADC_MUX 0b101101

/** Sampling frequency (after decimation if used) */
//#define ADC_SAMPLE_FREQUENCY 320


// ---------------------------------------------------------------------------
// FFT configuration - required by the fft module
// ---------------------------------------------------------------------------

/** Size of the fft in 2^n */
//#define FFT_M 6


// ---------------------------------------------------------------------------
// nvParam configuration relative to src/.
// ---------------------------------------------------------------------------
// #define NV_PARAM_DEFS_FILENAME "projectX/nvParamDefs.h"

#endif // ndef __CONFIG_H_HAS_ALREADY_BEEN_INCLUDED__

