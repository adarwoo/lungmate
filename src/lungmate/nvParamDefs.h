#ifndef __NV_PARAM_DEFINED__
#define __NV_PARAM_DEFINED__
// Note: DO NOT CHANGE THIS MACRO NAME
/**
 *@ingroup lungmate
 *@{
 *@file
 *****************************************************************************
 * Defines all non-volatile parameters used in this application
 *
 * @author software@arreckx.com
 *****************************************************************************
 */
#include "wgx.h"
#include "nvParam.h"

NV_PARAM_TABLE_BEGIN 

   /** 
    * Threshold in Watt from which to trigger the extraction.
    * The minimum should be greater than that the low threshold
    *  hysteris THRESHOLD_HYSTERISIS defined in stateMachine.c
    */
   NV_PARAM( powerThreshold,    "Turn on min power (watts)", 30, 9999, 50 )

   /** 
    * Select the mode (auto or off) to be selected when the device
    *  restarts (following a power cut for example). 0 for off
    *  or 1 for automatic mode. Default is auto.
    */
   NV_PARAM( restartInAutoMode, "Reset mode (1=auto, 0=off)", 0, 1, 1 ) 

   /** 
    * Delay in second to keep the dust extractor running for
    * after the load has gone
    */
   NV_PARAM( keepOnAfter,       "Post delay (seconds)",  0, 99, 5 )

   /** 
    * Ratio of the FFT measurement to watt.
    * This parameter can be used to adjust the power measurement, for example
    *  to allow for the power factor correction.
    * For example, to increase the measure value by 10%, multiply the current
    *  value by 1.1 and round to the nearest. For 3380 (default) this would
    *  be 3718.
    */
   NV_PARAM( fftToWattRatio,    "Calibration",  1, 32767, 3380 )

   /** Center frequency to measure */
   NV_PARAM( centerFrequency,   "Center frequency (Hz)", 40, 70, 50 )

NV_PARAM_TABLE_END

#endif /* ndef __NV_PARAM_DEFINED__ */
