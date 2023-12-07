/**
 *@ingroup lib
 *@defgroup nvparam Non-volatile Parameters API
 *@{
 *@file
 *****************************************************************************
 * Implements all functions to access non-volatile variable in the
 *  eeprom memory of the micro-controller. Uses a file called nvParam.hex for
 *  the simulation.
 * This file relies on the actual application definition file to exists.
 * By default, the file name is nvParamDefs.h but can be changed by setting
 *  the macro NV_PARAM_DEFS_FILENAME.
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

#include <string.h>

#include "wgx.h"


/*
 *  Include the actual definitions. By default, we assume the file is 'nvParamDefs.h' unless
 *   specified otherwise in the macro NV_DEFS_FILENAME
 */
#include "nvParam.h"


/*
 *   Reset the macros which up to now were used to 'define' the enum only.
 *   They now 'set' the strings in flash.
 */
#undef NV_PARAM_TABLE_BEGIN
#undef NV_PARAM_TABLE_END
#undef NV_PARAM

#define NV_PARAM_TABLE_BEGIN
#define NV_PARAM_TABLE_END

#define NV_PARAM(x,t,m,M,d)   const char nvString_ ## x [] PROGMEM = t;

// Allow re-inclusion
#undef __NV_PARAM_DEFINED__

#include NV_PARAM_DEFS_FILENAME


/*
 *   Reset the macros which up to now were used to 'define' the enum only.
 *   They now 'set' the value in flash.
 */
#undef NV_PARAM_TABLE_BEGIN
#undef NV_PARAM_TABLE_END
#undef NV_PARAM

#define NV_PARAM_TABLE_BEGIN nvParam_t PROGMEM nvParameters[] = {
#define NV_PARAM_TABLE_END };

#define NV_PARAM(x,t,m,M,d)   { nvString_ ## x,m,M,d },

// Allow re-inclusion
#undef __NV_PARAM_DEFINED__

#include NV_PARAM_DEFS_FILENAME


/*
 *   Start the actual implementation
 */


/**
 * Read a parameter from flash
 * @param index Position of the parameter.
 *              Use the enum value of the parameter requirered.
 *              It correspond to the parameter name with _e appended.
 * @return The parameter value given as a raw 32 bit number. Apply a cast
 *          as required or better, use a pre-cast function like nvParamInt, nvParamBool or nvParamFloat
 */
nvParamData_t nvParamGetValue( size_t index )
{
   nvParamData_t retval;

   // Calculate address of the value and return
   void *eeprom = (void *)(0/*begining*/ + sizeof(uint16_t)/*skip checksum*/ + sizeof(nvParamData_t) * index);

   // Copy the data literally
   eeprom_read_block( (void *)&retval, eeprom, sizeof(nvParamData_t) );

   return retval;
}


/**
 * Internal method which writes a new value at the desire location
 * Make sure the value as been checked against the contraints programmed
 * @param data  Passed in as a dword, and written as is to the eeprom
 * @param index Index of the parameter
 */
static void nvParamWriteValue( nvParamData_t data, size_t index )
{
   // Calculate address of the value and return
   void *eeprom = (uint32_t*)(0/*begining*/ + sizeof(uint16_t)/*skip checksum*/ + sizeof(nvParamData_t) * index);

   // Copy the data literally
   eeprom_write_block( (const void *)&data, eeprom, sizeof(nvParamData_t) );
}


/**
 * Set a new integer parameter value
 *
 * @param index Index of the data to set
 * @param newValue New value to write
 * @return A status with NV_OK on success.
 */
nvSetStatus_t nvParamSet( size_t index, nvParamData_t newValue )
{
   nvParam_t param;
   nvSetStatus_t retval = nvOK_e;

   if ( index < nvEndOfParamEnum_e )
   {
      // Read from flash
      memcpy_P( &param, &nvParameters[index], sizeof(nvParam_t) );

      if ( newValue > param.max )
      {
         retval = nvNumberTooLarge_e;
      }
      else if ( newValue < param.min )
      {
         retval = nvNumberTooSmall_e;
      }
      else
      {
         nvParamWriteValue( newValue, index );
      }
   }
   else
   {
      retval = nvIndexOutOfRange_e;
   }

   return retval;
}


/**
 * Compute the checksum of the nv configuration
 * The checksum is a digest of the number of arguments,their
 * minimum and maximum values.
 * It is written at the end of the table in eeprom to make sure
 * that the eeprom content match the expected parameter schema.
 *
 * @return The checksum of the configuration
 */
static uint16_t nvParamCheckSum(void) {
   size_t i;
   size_t j;
   uint16_t sum = 0;

   uint32_t r = 55665;
   static const uint16_t c1 = 52845, c2 = 22719;

   for ( i=0; i < nvEndOfParamEnum_e; ++i )
   {
      for ( j=0; j<sizeof(nvParam_t); ++j )
      {
         uint8_t byte = pgm_read_byte( ((uint8_t*)&nvParameters[i]) + j );

         uint8_t cipher = byte ^ (r >> 8);
         r = ((cipher + r) * c1) + c2;
         sum += cipher;
      }
   }

   return sum;
}


/**
 * Wipe the eeprom and reset all values using the programmed defaults
 */
void nvParamResetAll(void)
{
   // To iterate through each parameters
   size_t i;

   // Eeprom organised in bytes for clarity
   // Skip the checksum to start with
   uint8_t *eeprom = (void *)sizeof(uint16_t);

   for ( i=0; i < nvEndOfParamEnum_e; ++i )
   {
      // Get hold of the non-volatile parameter default value address
      uint32_t initialValue = pgm_read_dword( &(nvParameters[i].initial) );

      // And write as is to the eeprom
      eeprom_write_dword( (uint32_t*)eeprom, initialValue );

      // Move eeprom pointer too
      eeprom += sizeof(nvParamData_t);
   }

   // Get the checksum - but only write once all the data is written
   uint16_t cksum = nvParamCheckSum();

   // Now write the checksum
   eeprom_write_word( (uint16_t*)0, cksum );
}


/**
 * Initialise this API and reset the non-volatile memory if required.
 * The eeprom on the micro-controller is activated, and its content checked.
 * Each parameter definition will have a different checksum. If the checksum found in the
 *  eeprom is different, it means that the eeprom content is incorrect.
 * The eeprom is then reset using the default values provided, with the checksum written last.
 */
void nvParamInit(void)
{
   // Check the eeprom integrity by reading the first 2 bytes and comparing against the checksum
   uint16_t cksum = nvParamCheckSum();

   // Does it match the eeprom content
   if ( cksum != (uint16_t)eeprom_read_word( (uint16_t*)0 ) )
   {
      // Reset the eeprom
      nvParamResetAll();
   }
}

/**
 *  Return the range for a given param.
 *
 *  @param index Index of the parameter
 *  @param min Pointer receiving the lower limit inclusive
 *  @param max Pointer receiving the upper limit inclusive
 *
 *  @return A pointer to the parameter name as a flash string
 */
PGM_P nvGetRange( size_t index, nvParamData_t *min, nvParamData_t *max )
{
   nvParam_t param;

   // Read from flash
   memcpy_P( &param, &nvParameters[index], sizeof(nvParam_t) );

   *min = param.min;
   *max = param.max;

   return param.label;
}


/* ----------------------------  End of file  ---------------------------- */

