/**
 *@ingroup lungmate
 *@{
 *@file
 *****************************************************************************
 * This file defines the fuse and lock bits standard configuration for
 *  the AVR for the lungmate.
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

#include "wgx.h"

//
// Create fuses and lock bits sections in the elf file
//

/** No memory lock features enabled. */
LOCKBITS = LB_MODE_1;

/**
 *  Set the fuses.
 *  <ul>
 *    <li>External crystal > 8MHz with BOD active</li>
 *    <li>Watchdog is on and cannot be disabled</li>
 *    <li>EEPROM memory is preserved through a Chip Erase</li>
 *    <li>Brown-out Detector trigger level at 4.3V (typ)</li>
 * </ul>
 */
FUSES = {
    .low = LFUSE_DEFAULT,
    .high = (FUSE_WDTON & FUSE_EESAVE & FUSE_BODLEVEL2),
    .extended = (FUSE_CKSEL3 & FUSE_CKSEL2 & FUSE_CKSEL1 & FUSE_CKSEL0 & FUSE_SUT0 & EFUSE_DEFAULT)
};

