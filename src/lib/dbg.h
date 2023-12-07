#ifndef __DBG_H__HAS_ALREADY_BEEN_INCLUDED__
#define __DBG_H__HAS_ALREADY_BEEN_INCLUDED__
/**
 *@ingroup lib
 *@defgroup dbg Bit Tracing API
 *@{
 *@file
 *****************************************************************************
 * API to help debugging by waggling up and down I/O on the microcontroller
 *
 * Defining an unused I/O allow it to be used for debugging purpose, but also
 *  will configure the pin correctly at run-time.
 *
 * This library assumes that the pins defined for debugging are use for this
 *  purpose and are not shared. As such the default value of the port upon
 *  reset (DDR=PORT=0) are assumed. If the pin is shared somehow you must
 *  clear the DDR and PORT explicitly.
 *
 * To define an I/O as a debug (or unused pin), simply define the following
 *  macros in the config.h: DBG_x_PORT and DBG_x_BIT
 *   where x is an integer from 0 to 7 which uniquely defines how many 
 *   debug pins are available to the system.
 * The identifier x must be declared continously (no gaps) from 0 to n.
 *
 * To use a debug pin, simply specify the pin number using one of the dbgSet
 *  or dbgClear macros. 
 * You can specify a debug pin function in the config file. This make
 *  referencing clearer and help maintain the code - for example:
 * // Assign a debug pin a function in config.h allong with the pin targeted
 * #define DBG_ADC_CONV 4
 * // Then use in the code
 * dbgSet(DBG_ADC_CONV);
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

#include "wgx.h"

/* Define the debug pool size */
#if defined DBG_0_PORT && defined DBG_0_BIT
   #if defined DBG_1_PORT && defined DBG_1_BIT
      #if defined DBG_2_PORT && defined DBG_2_BIT
         #if defined DBG_3_PORT && defined DBG_3_BIT
            #if defined DBG_4_PORT && defined DBG_4_BIT
               #if defined DBG_5_PORT && defined DBG_5_BIT
                  #if defined DBG_6_PORT && defined DBG_6_BIT
                     #if defined DBG_7_PORT && defined DBG_7_BIT
                        #define DBG_POOL_SIZE 8
                     #else
                        #define DBG_POOL_SIZE 7
                     #endif
                  #else
                     #define DBG_POOL_SIZE 6
                  #endif
               #else
                  #define DBG_POOL_SIZE 5
               #endif
            #else
               #define DBG_POOL_SIZE 4
            #endif
         #else
            #define DBG_POOL_SIZE 3
         #endif
      #else
         #define DBG_POOL_SIZE 2
      #endif
   #else
      #define DBG_POOL_SIZE 1
   #endif
#else
   #define DBG_POOL_SIZE 0
#endif

// In super release mode, the debug led is left off to avoid causing 
//  current fluctuations. The port is configured as input with the
//  internal pull-up activated to reduce power consumption
#ifdef NDEBUG
   /** Simply turn on the pull up to stabilise the pin. We
    *  assume the port function has not been altered after chip
    *  reset and that the DDR is set to 0 */
   #define DBG_INIT_POOL(n) \
      { PIN_OF(DBG_##n##_PORT) |= _BV(DBG_##n##_BIT); } 

   static inline void dbgSet(size_t n) {}
   static inline void dbgClear(size_t n) {}
#else
   /** Each defined port is configured as an output. We assume the
    *  port initial value is left untouched as 0 */
   #define DBG_INIT_POOL(n) \
      { DDR_OF(DBG_##n##_PORT) |= _BV(DBG_##n##_BIT); } 

   /** 
    * Sets the designated debug pin. The optimizer will reduce this 
    * expression down to single assembler instruction. 
    */
   static inline void dbgSet( const size_t n )
   {
      #if DBG_POOL_SIZE == 0
         #warning "Attempt to use debug methods but no debug pins are defined correctly"
      #else
         if ( n==0 ) DBG_0_PORT |= _BV(DBG_0_BIT);
      #endif
      #if DBG_POOL_SIZE > 1
         if ( n==1 ) DBG_1_PORT |= _BV(DBG_1_BIT);
      #endif
      #if DBG_POOL_SIZE > 2
         if ( n==2 ) DBG_2_PORT |= _BV(DBG_2_BIT);
      #endif
      #if DBG_POOL_SIZE > 3
         if ( n==3 ) DBG_3_PORT |= _BV(DBG_3_BIT);
      #endif
      #if DBG_POOL_SIZE > 4
         if ( n==3 ) DBG_4_PORT |= _BV(DBG_4_BIT);
      #endif
      #if DBG_POOL_SIZE > 5
         if ( n==3 ) DBG_5_PORT |= _BV(DBG_5_BIT);
      #endif
      #if DBG_POOL_SIZE > 6
         if ( n==3 ) DBG_6_PORT |= _BV(DBG_6_BIT);
      #endif
      #if DBG_POOL_SIZE > 7
         if ( n==3 ) DBG_7_PORT |= _BV(DBG_7_BIT);
      #endif
   }

   /** 
    * Sets the designated debug pin. The optimizer will reduce this 
    * expression down to single assembler instruction. 
    */
   static inline void dbgClear( const size_t n )
   {
      #if DBG_POOL_SIZE == 0
         #warning "Attempt to use debug methods but no debug pins are defined correctly"
      #else
         if ( n==0 ) DBG_0_PORT &= ~_BV(DBG_0_BIT);
      #endif
      #if DBG_POOL_SIZE > 1
         if ( n==1 ) DBG_1_PORT &= ~_BV(DBG_1_BIT);
      #endif
      #if DBG_POOL_SIZE > 2
         if ( n==2 ) DBG_2_PORT &= ~_BV(DBG_2_BIT);
      #endif
      #if DBG_POOL_SIZE > 3
         if ( n==3 ) DBG_3_PORT &= ~_BV(DBG_3_BIT);
      #endif
      #if DBG_POOL_SIZE > 4
         if ( n==3 ) DBG_4_PORT &= ~_BV(DBG_4_BIT);
      #endif
      #if DBG_POOL_SIZE > 5
         if ( n==3 ) DBG_5_PORT &= ~_BV(DBG_5_BIT);
      #endif
      #if DBG_POOL_SIZE > 6
         if ( n==3 ) DBG_6_PORT &= ~_BV(DBG_6_BIT);
      #endif
      #if DBG_POOL_SIZE > 7
         if ( n==3 ) DBG_7_PORT &= ~_BV(DBG_7_BIT);
      #endif
   }

   /** 
    * Sets the designated debug pin. The optimizer will reduce this 
    * expression down to single assembler instruction. 
    */
   static inline void dbgToggle( const size_t n )
   {
      #if DBG_POOL_SIZE == 0
         #warning "Attempt to use debug methods but no debug pins are defined correctly"
      #else
         if ( n==0 ) DBG_0_PORT ^= _BV(DBG_0_BIT);
      #endif
      #if DBG_POOL_SIZE > 1
         if ( n==1 ) DBG_1_PORT ^= _BV(DBG_1_BIT);
      #endif
      #if DBG_POOL_SIZE > 2
         if ( n==2 ) DBG_2_PORT ^= _BV(DBG_2_BIT);
      #endif
      #if DBG_POOL_SIZE > 3
         if ( n==3 ) DBG_3_PORT ^= _BV(DBG_3_BIT);
      #endif
      #if DBG_POOL_SIZE > 4
         if ( n==3 ) DBG_4_PORT ^= _BV(DBG_4_BIT);
      #endif
      #if DBG_POOL_SIZE > 5
         if ( n==3 ) DBG_5_PORT ^= _BV(DBG_5_BIT);
      #endif
      #if DBG_POOL_SIZE > 6
         if ( n==3 ) DBG_6_PORT ^= _BV(DBG_6_BIT);
      #endif
      #if DBG_POOL_SIZE > 7
         if ( n==3 ) DBG_7_PORT ^= _BV(DBG_7_BIT);
      #endif
   }

#endif // def NDEBUG

/** 
 * Initialise the ports and pins used for debugging.
 * The actual implementation dedends on the NDEBUG flag. If this
 *  flag is set to 1, the init function will set all ports I/O as
 *  input with the internal pull-up activated. This is the recommended
 *  setting for unused pins. If NDEBUG is undefined or < 1, the I/O will
 *  be configured as output.
 */
static inline void dbgInit(void)
{
   #if DBG_POOL_SIZE > 0
      DBG_INIT_POOL(0)
   #endif
   #if DBG_POOL_SIZE > 1
      DBG_INIT_POOL(1)
   #endif
   #if DBG_POOL_SIZE > 2
      DBG_INIT_POOL(2)
   #endif
   #if DBG_POOL_SIZE > 3
      DBG_INIT_POOL(3)
   #endif
   #if DBG_POOL_SIZE > 4
      DBG_INIT_POOL(4)
   #endif
   #if DBG_POOL_SIZE > 5
      DBG_INIT_POOL(5)
   #endif
   #if DBG_POOL_SIZE > 6
      DBG_INIT_POOL(6)
   #endif
   #if DBG_POOL_SIZE > 7
      DBG_INIT_POOL(7)
   #endif
}

#endif   /* ndef __DBG_H__HAS_ALREADY_BEEN_INCLUDED__ */

