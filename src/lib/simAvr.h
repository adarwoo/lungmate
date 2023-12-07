#ifndef __WINSIM_HAS_BEEN_INCLUDED__HEADER__
#define __WINSIM_HAS_BEEN_INCLUDED__HEADER__
/**@ingroup sim*/
/**@file
 * Compatibility header to compile some of the native AVR files
 *  on a Windows system using Cygwin with the gcc compiler.
 */

#include <inttypes.h>
#include <stdlib.h>

/*
 *  Flash memory access is simply discarded in the simulation
 */
#define PGM_P const char *
#define PSTR(x) ((const char *)x)
#define memcpy_P memcpy
#define strlen_P strlen
#define strcmp_P strcmp
#define pgm_read_byte(a) (*a)
#define pgm_read_dword(a) (*a)
#define PROGMEM

/*
 * I/O Registers as static variables
 */
extern uint8_t PORTA;
extern uint8_t PORTB;
extern uint8_t DDRA;
extern uint8_t DDRB;
extern uint8_t PINA;
extern uint8_t PINB;
extern uint8_t GIMSK;
extern uint8_t PCMSK0;
extern uint8_t PCMSK1;
extern uint8_t SREG;

// Bits
#define _BV(n) (1<<n)

static const uint8_t PCIE0=4;
static const uint8_t PCIE1=5;

static const uint8_t PORT0=0;
static const uint8_t PORT1=1;
static const uint8_t PORT2=2;
static const uint8_t PORT3=3;
static const uint8_t PORT4=4;
static const uint8_t PORT5=5;
static const uint8_t PORT6=6;
static const uint8_t PORT7=7;

static const uint8_t IBIT=7;

static inline void cli() {
   SREG &= ~_BV(IBIT);
}

static inline void sei() {
   SREG |= _BV(IBIT);
}

#define bit_is_clear(p,b) (p&b==0)
#define bit_is_set(p,b) (p&b!=0)

static inline void set_sleep_mode(uint8_t mode) {
   (void)mode;
}

static inline void sleep_mode() {
}

static inline void wdt_reset() {
}

static inline void power_usi_disable() {
}

#define ISR(f) void f()

/*
 * AVR library has some non-standard function
 */
int printf_P( PGM_P format, ... );
char *itoa (int val, char *s, int radix);
#define square(a) (((float)a)*((float)a))

/*
 *  Eeprom emulation
 */
#define EEPROM_SIZE   512

#define  eeprom_is_ready()
#define  eeprom_busy_wait()   do {} while (!eeprom_is_ready())

uint8_t  eeprom_read_byte (const uint8_t *__p);
uint16_t eeprom_read_word (const uint16_t *__p);
uint32_t eeprom_read_dword (const uint32_t *__p);
void  eeprom_read_block (void *__dst, const void *__src, size_t __n);
void  eeprom_write_byte (uint8_t *__p, uint8_t __value);
void  eeprom_write_word (uint16_t *__p, uint16_t __value);
void  eeprom_write_dword (uint32_t *__p, uint32_t __value);
void  eeprom_write_block (const void *__src, void *__dst, size_t __n);

/*
 * Force immediate flush on all characters
 */
#undef putchar
#define putchar(c) printf("%c", c);fflush(stdout)


#endif // ndef __WINSIM_HAS_BEEN_INCLUDED__HEADER__
