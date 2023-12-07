#ifndef __KEY_H__HAS_ALREADY_BEEN_INCLUDED__
#define __KEY_H__HAS_ALREADY_BEEN_INCLUDED__
/**
 *@ingroup key
 *@{
 *@file
 *****************************************************************************
 * Defines the single key API.
 *
 * The key API must be initialised by calling keyInit.
 * The KEY_PORT and KEY_PIN resource must be defined in config.h.
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

/** Prefered key sampling rate in Hz*/
#define KEY_PREFERED_SCANNING_RATE 50

/** 
 * Callback type to be passed in the init method called 
 *  on a short or long push
 */
typedef void (*keyPushedCallback_t)(void);

void keyInit(
   keyPushedCallback_t shortPushCallBack, 
   keyPushedCallback_t longPushCallBack );

void keyScan(void);


#endif   /* ndef __KEY_H__HAS_ALREADY_BEEN_INCLUDED__ */
