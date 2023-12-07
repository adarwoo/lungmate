#ifndef __CONSOLE_H__HAS_ALREADY_BEEN_INCLUDED__
#define __CONSOLE_H__HAS_ALREADY_BEEN_INCLUDED__
/**
 *@ingroup lib
 *@defgroup console Console API
 *@{
 *@file
 *****************************************************************************
 * Defines the method to initiate the interactive console.
 * The console relies on the uart and the non-volatile parameter API to work.
 *
 * @author software@arreckx.com 
 *****************************************************************************
 */

/*- Special ASCII Control characters */

/** Back space ASCII - cursor goes back by one */
#define BS   8

/** Bell ASCII value - beeps when printed */
#define BEL  7

/** Carriage return ASCII value */
#define CR  13

/** Line feed ASCII value */
#define LF  10


/**
 * Maximum number of characters required to display a 16bit 
 * unsigned number in base 10
 */
#define CONSOLE_MAX_CHARACTERS_IN_UINT16 5

/**
 * Maximum number of characters required to display a 16bit number in base 10
 */
#define CONSOLE_MAX_CHARACTERS_IN_INT16  6

/** 
 * Helper that prints a error message
 * @param str String to use for the error message. This string 
 *            will be printed with 'Err:' as a prefix
 */
#define consoleError( str ) uartPrintln( PSTR("Err:" str ) )

void consolePrompt(void);


#endif   /* ndef __CONSOLE_H__HAS_ALREADY_BEEN_INCLUDED__ */

