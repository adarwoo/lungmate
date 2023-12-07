#ifndef __FFT_H_HAS_ALREADY_BEEN_INCLUDED__
#define __FFT_H_HAS_ALREADY_BEEN_INCLUDED__
/**
 *@ingroup fft
 *@{
 *@file
 *****************************************************************************
 * This FFT calculates the PSD of a single frequency in the spectrum.
 * Unlike standard algorithm, this FFT is computed progressively, 1 sample
 *  at a time.
 * This API must first be initialised with fftInit providing which
 *  frequency to measure.
 * Each new sample should be evaluated with fftNext which returns true
 *  as soon as an actual fft measurement is ready. The results can then be
 *  retreived by calling fftGetResult.
 * The result is presented as a raw float of the computed PSD and must be
 *  adjusted accoringly.
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

#include "wgx.h"

void  fftInit(uint16_t centerFrequency);
void  fftReset(void);
bool  fftNext(int16_t sample);
float fftGetResult(void);


#endif   /* ndef __FFT_H_HAS_ALREADY_BEEN_INCLUDED__ */
