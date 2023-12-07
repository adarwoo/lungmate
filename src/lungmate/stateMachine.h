#ifndef __STATEMACHINE_H__HAS_ALREADY_BEEN_INCLUDED__
#define __STATEMACHINE_H__HAS_ALREADY_BEEN_INCLUDED__
/**
 *@ingroup lungmate
 *@{
 *@file
 *****************************************************************************
 * Defines the state machine API.
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

#include "wgx.h"

void smInit(void);

void smProcessFFTResult(uint16_t value);
void smProcessTick(void);
void smProcessShortKey(void);
void smProcessLongKey(void);


#endif   /* ndef __STATEMACHINE_H__HAS_ALREADY_BEEN_INCLUDED__ */
