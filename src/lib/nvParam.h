#ifndef __NV_PARAM_H_HAS_ALREADY_BEEN_INCLUDED__
#define __NV_PARAM_H_HAS_ALREADY_BEEN_INCLUDED__
/**
 *@ingroup nvparam
 *@{
 *@file
 *****************************************************************************
 * Defines the non-volatile variable access API.
 * Files using the nvParam API should not include this header, but rather the
 *  definition files which lists all non-volatile parameters.
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

#include "wgx.h"

#ifndef NV_PARAM_DEFS_FILENAME
#error "NV_PARAM_DEFS_FILENAME is not defined. Define it in your config file pointing to the param definition file"
#endif

/**
 * Abstract data type to store the parameter. Could be made bigger if required.
 */
typedef int16_t nvParamData_t;

/**
 * Definition of a single parameter
 */
typedef struct {
   /** Parameter label to display in console */
   PGM_P label;
   /** Minimum (included) value for the parameter */
   nvParamData_t min;
   /** Maximum (included) value for the parameter */
   nvParamData_t max;
   /** Default value for this parameter when the eeprom is reset */
   nvParamData_t initial;
} nvParam_t;

/** Return status in all call to nvParamXXX methods */
typedef enum {
   /** All is fine. Function call is a success */
   nvOK_e=0,
   /** The given index is out of range. The index should be positive and less that the total number of parameters defined in the table */
   nvIndexOutOfRange_e,
   /** New value over permitted range */
   nvNumberTooLarge_e,
   /** New value under permitted range */
   nvNumberTooSmall_e,
} nvSetStatus_t;

/** Starts the defintion of the parameter definition table. Only 1 instance of this macro should be used throughout */
#define NV_PARAM_TABLE_BEGIN typedef enum {

/** Terminates the parameter definition table */
#define NV_PARAM_TABLE_END nvEndOfParamEnum_e } nvParamEnum_t;

/**
 * Adds a integer parameter to the definition table.
 * Must be used within the NV_PARAM_TABLE_BEGIN and NV_PARAM_TABLE_END macros
 * @param x Identifier for this parameter. A new enum value x_e will be created.
 *          This enum value can then be used to access this parameter
 * @param m Minimum, inclusive, value of the paramter
 * @param M Maximum inclusive value of the parameter
 * @param d Default value to use when the eeprom table is not defined
 */
#define NV_PARAM(x,t,m,M,d)   x##_e,

#include NV_PARAM_DEFS_FILENAME

/** Shortcut to get the int value of the parameter at the given index  */
#define nvParam(index) (int)(nvParamGetValue(index))


void nvParamInit(void);
nvSetStatus_t nvParamSet( size_t index, nvParamData_t newValue );
void  nvParamResetAll(void);
nvParamData_t nvParamGetValue( size_t index );
PGM_P nvGetRange( size_t index, nvParamData_t *min, nvParamData_t *max );


/** 
 *  Returns the number of parameters in the table
 *  @return  The number of parameters
 */
#define nvGetSize() (size_t)nvEndOfParamEnum_e


#endif // ndef __NV_PARAM_H_HAS_ALREADY_BEEN_INCLUDED__
