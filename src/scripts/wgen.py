#!/usr/bin/env python

#
# Compute the twiddle factor for different size fft, and create some
#  lookup table in C for AVR
#
#
#

import sys
from math import *

# The fisrt argument is taken to be the output file
if len(sys.argv) > 1:
    out = open(sys.argv[1], "w")
else:
    out = sys.__stdout__

# Prepare the file header
out.write("""/*-
/**@file
 *****************************************************************************
 * Specifies twiddle factors for the fft.
 * This file was generated by %s - DO NOT EDIT this file.
 * This file should be included by the application where the FFT algorithm
 *  is implements and not compiled directly.
 *
 * This files defines all twiddle factors coefficients as lookup tables
 *  stored in flash memory.
 *  The table selected depends on the size of the FFT which must be defined
 *   in the pre-processor variable FFT_N. If not specified, the FFT is 64pt.
 * The complex type must be defined as follow:
 * typedef struct { float real; float imag; } complex_t;
 *****************************************************************************
 */

/* Read the size of the FFT */
#ifndef FFT_N
#  define FFT_N 64
#endif

static complex_t fftTwiddle[] PROGMEM = {
""" % sys.argv[0])

# Compute for fft ranging from 8pt to 256 points
for fftSize in [ 8, 16, 32, 64, 128, 256 ]:

    # Write the preprocessor header for each fft size
    out.write("#  if ( FFT_N == %d )\n" % fftSize);

    # Pre-compute Twiddle factors Ws
    sector=2*pi/fftSize
    for i in range(0,fftSize/2):
        out.write( "   { % .6f, % .6f }" % (cos(i*sector), sin(i*sector) ) )
        if i<fftSize/2-1:
            out.write( "," )
        out.write( "\n" )

    # Close the preprocessor header
    out.write("#  endif /* FFT_N == %d */\n" % fftSize);

# Close the file
out.write( "};\n" )

