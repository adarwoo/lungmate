#ifndef __WGX_H_HAS_ALREADY_BEEN_INCLUDED__
#define __WGX_H_HAS_ALREADY_BEEN_INCLUDED__
/**
 *@defgroup lib WGX Library
 *@ingroup lib
 *@{
 *@file
 *****************************************************************************
 * This header file should be included in all files as it defines the standard
 *  and target specific types used throughout.
 * It also include all of the project definitions for ports, timing etc...
 * It can be included by the target or simulator code alike.
 *
 * It should be the first header included by all c files if it is to be used
 *  as the pre-compiled header.
 * 
 * @page lib AVR reusable library
 * This library contains a collection of simple software designed to work
 *  closely with the AVR. They do not depend on some thirdparty OS but simply
 *  on the standart C library that comes with the AVR GCC.
 * Each module requires that macros be defined in the configuration header to
 *  operate correctly.
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

#include <inttypes.h>

/** boolean type, from C99 standard. avr-gcc lacks the stdbool header. */
typedef _Bool bool;

/** Boolean true macro. Similar to stdbool declaration */
#define true ((bool)1)

/** Boolean false macro. Similar to stdbool declaration */
#define false ((bool)0)

/** Include all target specific headers or the simulated version */
#ifdef AVR
   #include <avr/eeprom.h>
   #include <avr/pgmspace.h>
   #include <avr/io.h>
   #include <avr/sleep.h>
   #include <avr/wdt.h>
   #include <avr/interrupt.h>
   #include <avr/power.h>
#else
   #include "simAvr.h"
#endif


/**
 * Helper macros for expanding DDR, PIN etc. from the PORT All
 *  of these will be expanded inline, and optimized such that
 *  the remaining code will be as is the correct port was used
 *  in place
 *  Do not use directly, but use the macro DDR_OF instead
 *  @param port Address of the port. (e.g. &PORTA)
 */
static inline volatile uint8_t *wgxDDROf( volatile uint8_t *port )
{
   if ( port==&PORTA ) return &DDRA;
   return &DDRB;
}

static inline volatile uint8_t *wgxPinOf( volatile uint8_t *port )
{
   if ( port==&PORTA ) return &PINA;
   return &PINB;
}

static inline volatile uint8_t *wgxPCMSKOf( volatile uint8_t *port )
{
   if ( port==&PORTA ) return &PCMSK0;
   return &PCMSK1;
}

/**
 * Convert the given PORT into its matching DDR
 *  Example: DDR_OF(PORTA) = 0;
 *  @param port Actual port.
 */
#define DDR_OF(port) *wgxDDROf(&port)

/** Convert the given PORT into its matching PIN */
#define PIN_OF(port) *wgxPinOf(&port)

/** Convert the given PORT into its matching PCMSK */
#define PCMSK_OF(port) *wgxPCMSKOf(&port)


/*
 * Include all hardware and constant definitions
 */
#ifndef CFG
   #include "cfg/cfg.h"
#else
   #define STR(x) #x
   #define PATH(x) STR(cfg/x.h)
   #include PATH(CFG)
#endif


#endif // ndef __WGX_H_HAS_ALREADY_BEEN_INCLUDED__
