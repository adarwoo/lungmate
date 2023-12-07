#ifndef __PREAMBULE_H__HAS_ALREADY_BEEN_INCLUDED__
#define __PREAMBULE_H__HAS_ALREADY_BEEN_INCLUDED__
/**@file
 *****************************************************************************
 * The preambule defines all methods and hooks to be called before the main
 *  application starts.
 * Some of the functions implemented by preambule.c are actually run before
 *  the application main is called.
 * The preambule mathod should be the first thing to call in main.
 * After this call, all managed API should be running.
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

void preamble(void);

#endif   /* ndef __ADC_H__HAS_ALREADY_BEEN_INCLUDED__ */

