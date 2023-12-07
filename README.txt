Project lungmate

Author: software@arreckx.com
==============================================================================

This directory contains the source files and CAD files for the lungmate
 project.
Lungmate is an electronic device, housed in a circuit breaker like case which
 controls the dust extractor of a woodworking workshop.

The detail documentation is not available from this project as it is my
 intention to publish it in Elektor magazine.

You will find, however, all of the data such as the software and the PCB files.

The PCB data was created using Eagle (www.cadsoft.de) version 5.6 and the PCB
 was built by PCB-POOL services.

The software was compiled using the winavr software suite under Windows.
It should possible to build it under Linux with no changes, including some
 of the simulation builds.

Software requirement for Windows build
--------------------------------------
Cygwin 1.6 or later. (Note not tested under 1.7.1). 
	Make sure to install gcc, mintty, python and make.
WinAvr 2009-03-13 or later from http://sourceforge.net/projects/winavr
Doxygen and graphviz for the documentation
AVRStudio can be used too.

Building the software
---------------------
The software should be built from a shell. In Cygwin, mintty is a good choice.
To build the lungmate software type:
make NDEBUG=1

The final build product can be found in dist/lungmate_release.hex
and should include the fuse and lock bits data too.

To build the unit tests, type:
make test

To build the simulation:
make sim

To build all:
make all

To find the list of all the targets:
make list

To build the code documentation using Doxygen:
make doc
