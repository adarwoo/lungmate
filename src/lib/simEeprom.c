/**
 *@ingroup lib
 *@defgroup sim PC Simulation
 *@{
 *@file
 *****************************************************************************
 * Provides all stub code required to simulate the eeprom
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

#ifdef AVR
#  error "PC simulator only"
#endif

#include <string.h>

#include "wgx.h"

/*
 *  Eeprom emulation
 *  A file is used to write everything
 */
static char eeprom[EEPROM_SIZE];

static bool eepromHasBeenInitialised = false;

/** Initialize the eeprom which is simulated using RAM */
static inline void eeprom_preambule()
{
   if ( ! eepromHasBeenInitialised )
   {
      memset( eeprom, 0xff, 512 );
      eepromHasBeenInitialised = true;
   }
}

uint8_t eeprom_read_byte (const uint8_t *__p)
{
   int i=(int)__p;
   eeprom_preambule();
   return *((uint8_t *)(&eeprom[i]));
}

uint16_t eeprom_read_word (const uint16_t *__p)
{
   int i=(int)__p;
   eeprom_preambule();
   return *((uint16_t *)(&eeprom[i]));
}

uint32_t eeprom_read_dword (const uint32_t *__p)
{
   int i=(int)__p;
   eeprom_preambule();
   return *((uint32_t *)(&eeprom[i]));
}

void eeprom_read_block (void *__dst, const void *__src, size_t __n)
{
    void *p = eeprom + (int)__src;
    eeprom_preambule();
    memcpy(__dst, p, __n);
}

void eeprom_write_byte (uint8_t *__p, uint8_t __value)
{
   int i=(int)__p;
   eeprom_preambule();
   *((uint8_t *)(&eeprom[i])) = __value;
}

void eeprom_write_word (uint16_t *__p, uint16_t __value)
{
   int i=(int)__p;
   eeprom_preambule();
   *((uint16_t *)(&eeprom[i])) = __value;
}

void eeprom_write_dword (uint32_t *__p, uint32_t __value)
{
   uint32_t *p=(uint32_t *)&eeprom[(int)__p];
   eeprom_preambule();
   *p = __value;
}

void eeprom_write_block (const void *__src, void *__dst, size_t __n)
{
   char *p = eeprom + (int)__dst;
   eeprom_preambule();
   memcpy(p, __src, __n);
}


/* -----------------------------  End of file  ---------------------------- */

