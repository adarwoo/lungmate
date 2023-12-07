/**
 *@ingroup lib
 *@defgroup sim PC Simulation
 *@{
 *@file
 *****************************************************************************
 * Provides all stub code required to run some of lungmate files in a simulator
 *
 * @author software@arreckx.com
 * @page simulation Simulation of AVR code
 * The build allow some of the libraries to be simulated on a PC running
 * Linux or Windows.
 * The make file provided can build a simulation target from existing source.
 * The wgx.h macro includes all headers required for the .c file to compile
 *  on the system used for development.
 * See the \ref sim section.
 *****************************************************************************
 */

#ifdef AVR
#  error "PC simulator only"
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#include "wgx.h"
#include "console.h"

char *strdup( const char *p );

// Registers
uint8_t PORTA=0;
uint8_t PORTB=0;
uint8_t DDRA=0;
uint8_t DDRB=0;
uint8_t PINA=0;
uint8_t PINB=0;
uint8_t GIMSK=0;
uint8_t PCMSK0=0xc8;
uint8_t PCMSK1=0xff;
uint8_t SREG=0;


// Non-ANSI replacement
char *itoa (int val, char *s, int radix) {
   int i = 30;
   for(; val && i ; --i, val /= radix) s[i] = "0123456789abcdef"[val % radix];
   return &s[i+1];
}

/** Emulate the printf method, but flush at the end and substitue %S for %s */
#include <stdarg.h>
int printf_P( PGM_P format, ... )
{
    char subst[] = "%S";
    char *result = NULL;
    va_list argptr;
    int retval = 0;
    char *newFormat = strdup(format);
    result = strstr( newFormat, subst );

    while ( result != NULL )
    {
        result[1]='s';
        result = strstr( result+1, subst );
    }

    va_start( argptr, format );

    retval = vprintf( newFormat, argptr );
    va_end( argptr );

    fflush(stdout);

    return retval;
}


/* -----------------------------  End of file  ---------------------------- */

