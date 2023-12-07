#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# Generic make rules for building AVR elf and hex files as well as native
#  exe's for simulating part of the code.
# This makefile should be left untouched and does not need editing. To use,
#  the project create <dirname>.mak files in each of the source
#  directory where builds are required.
# Global rules and settings should be specified in the optional config.mak
#  file which should be placed in the project root dir.
# This makefile use eval construct to dynamically create some of the build
#  rules for the final targets.
#
# Invoking:
# Simply type make <targetname>
# The target can be one of those specified in the .mak file using the makeHex
#  makeSim or makeTest macro, or hex to build all hex, make sim to build all
#  simulations or make test to build unit tests.
# The default target is hex.
#
# @author software@arreckx.com
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

# All rules are written for bash prefered over sh
SHELL=/bin/bash

# Override any settings with your own
-include config.mak

#-----------------------------------------------------------------------------
# Flags which could be specified in config.mak
#-----------------------------------------------------------------------------

# Number of the CPUs to fire the build on
ifdef CYGWIN
SYSCPUS?=$(shell regtool get '/machine/SYSTEM/CurrentControlSet/Control/Session Manager/Environment/NUMBER_OF_PROCESSORS')
else
SYSCPUS?=2
endif
# MPU to use - defaults to attiny861
MCU?=atmega128
# Output format
FORMAT?=ihex
# Optimization level. For AVR -Os is probably the best compromized all round
OPT?=s
# Default configuration
DEFAULT.CFG?=cfg

# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
DEBUGOPT?=stabs

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD?=-std=gnu99

# Place -D or -U options here
CDEFS?=
#CDEFS += -DREMOVE_FLASH_BYTE_SUPPORT
#CDEFS += -DREMOVE_EEPROM_BYTE_SUPPORT
#CDEFS += -DREMOVE_FUSE_AND_LOCK_BIT_SUPPORT
#CDEFS += -DREMOVE_AVRPROG_SUPPORT
#CDEFS += -DREMOVE_BLOCK_SUPPORT

# Place -I options here
CINCS?=

#-----------------------------------------------------------------------------
# Non-mutable constructs
#-----------------------------------------------------------------------------
CDEBUG = -g$(DEBUGOPT)
CWARN = -Wall -Wstrict-prototypes
CTUNING = -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CDEPS=-MMD -MP -MT $(BUILD.DIR)/$(*F).o -MF $(BUILD.DIR)/$(@F).d
#CEXTRA = -Wa,-adhlns=$(<:.c=.lst)
CFLAGS = $(CDEBUG) $(CDEPS) $(CDEFS) $(CINCS) -O$(OPT) $(CWARN) $(CSTANDARD) $(CTUNING) $(CEXTRA) $(CPPFLAGS)
#ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs
#Additional libraries.
# Minimalistic printf version
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min
# Floating point printf version (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt
PRINTF_LIB =
# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min
# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt
SCANF_LIB =
MATH_LIB = -lm

#-----------------------------------------------------------------------------
# Binaries shortcut
#-----------------------------------------------------------------------------

# Compiler
CC = avr-gcc
# Object conversion
OBJCOPY = avr-objcopy
# Object dump
OBJDUMP = avr-objdump
# Size
SIZE = avr-size
# Name mangler and ibject inspector
NM = avr-nm
# Move or rename a file
MV = mv -f
# Name of this Makefile (used for "make depend").
MAKEFILE = Makefile

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -I. $(CFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)

# External memory options

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# used for variables (.data/.bss) and heap (malloc()).
#EXTMEMOPTS = -Wl,-Tdata=0x801100,--defsym=__heap_end=0x80ffff

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# only used for heap (malloc()).
#EXTMEMOPTS = -Wl,--defsym=__heap_start=0x801100,--defsym=__heap_end=0x80ffff

EXTMEMOPTS =

#LDMAP = $(LDFLAGS) -Wl,-Map=$(TARGET).map,--cref
LDFLAGS = $(EXTMEMOPTS) $(LDMAP) $(MATH_LIB)


# Mute flag depend on VERBOSE
# To see what make is doing, set VERBOSE to anything
ifeq ($(VERBOSE),)
MUTE=@
SMUTE=set -
EMUTE=2>/dev/null
NOTHINGTODO=@true
else
MUTE=
SMUTE=set -x
EMUTE=
NOTHINGTODO=
endif

#  Define the color code for an ANSI terminal
ifndef NOCOLOR
cBlack =\033[30m
cRed   =\033[31m
cGreen =\033[32m
cOrange=\033[33m
cBlue  =\033[34m
cPurple=\033[35m
cCyan  =\033[36m
cGrey  =\033[37m
cError=$(cRed)
cWarn =$(cOrange)
cBuild=$(cGrey)
cInfo=$(cPurple)
cVar  =$(cCyan)
cReset=\033[0m
endif

# Manage build mode requested.
# Uses the variables DEBUG and NDEBUG
# The default (none defined) is standard debug.
# NDEBUG should be set to 1 to make a release build.
# DEBUG can be set to specify additional level of debugging required for the
#  build, like when using Insure++, Purify or others.
# DEBUG and NDEBUG are not permitted together
#
ifndef NDEBUG
ifndef DEBUG
BUILD.TYPE = debug
else
BUILD.TYPE = debug_$(DEBUG)
endif
else
ifdef DEBUG
ErrorMsg := DEBUG and NDEBUG are exclusive
endif
BUILD.TYPE=release
endif

# General tools.
ECHO = echo -e
PYTHON = python
MKDIRHIER := mkdir -p

BASEDIR=.

# All phony targets supported here
.PHONY : hex sim test all help

# Constants
DIST.DIR=dist
BUILD.BASE.DIR=build

OBJRULES:=

# Our default target
hex::

all: hex test sim

clean:
	$(MUTE)rm -rf $(DIST.DIR) $(BUILD.BASE.DIR)

.SUFFIXES: .elf .hex .eep .lss .sym

.elf.hex:
	$(MUTE)$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

.elf.eep:
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create extended listing file from ELF output file.
.elf.lss:
	$(MUTE)$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
.elf.sym:
	$(MUTE)$(NM) -n $< > $@

# Create the documentation
ifdef DOXYFILE
doc: $(DOXYFILE)
	$(MUTE)doxygen $(DOXYFILE)
endif

#-----------------------------------------------------------------------------
# Set of rules for when make is invoked the second time round with the project
#  defined
#-----------------------------------------------------------------------------
ifdef SRC.DIR

# Force destination directory creating
CREATE_DIST:=$(shell mkdir -p $(DIST.DIR) 2>/dev/null)

# Path where to find src and library source files
VPATH=$(SRC.DIR) src/lib

# Build directory where .o and .d to be placed
BUILD.DIR=$(BUILD.BASE.DIR)/$(CFG)_$(BUILD.TYPE)

# Create a list of all the obj files
OBJS=$(foreach c,$(CSRCS),$(BUILD.DIR)/$(notdir $(c:.c=.o)))

CPPFLAGS=-Isrc -Isrc/lib -I$(SRC.DIR) -DCFG=$(CFG)

# Keep the obj files please
.PRECIOUS : $(OBJS)

#
# Rules for compiling PC unit tests
#
ifeq ($(BUILD.TYPE),sim)
# Use regular compiler
override CC=gcc
$(DIST.DIR)/%.exe : $(OBJS)
	@$(ECHO) "$(cInfo). Building simulation $(cVar)$@$(cInfo)$(cReset)"
	$(MUTE)$(CC) -o $@ $^
	@chmod +x $@

# Pattern rules - VPATH is used to locate the source
$(BUILD.DIR)/%.o : %.c
	@$(ECHO) "$(cBuild)+ Compiling $(cVar)$<$(cBuild) to $(cVar)$@$(cBuild)...$(cReset)"
	$(MUTE)$(CC) -c -g -O0 $(CPPFLAGS) $< -Wall -o $@
else
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# AVR Section
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

vpath %.hex $(DIST.DIR)
vpath %.elf $(DIST.DIR)

.PRECIOUS : $(DIST.DIR)/%.elf

$(DIST.DIR)/%.elf : $(OBJS)
	@$(ECHO) "$(cInfo). Building $(cVar)$@$(cInfo)$(cReset)"
	$(MUTE)$(CC) -o $@ $^ -mmcu=$(MCU) $(LDFLAGS)

$(BUILD.DIR)/%.o : %.c
	@$(ECHO) "$(cBuild)+ Cross-Compiling $(cVar)$<$(cBuild) to $(cVar)$@$(cBuild)...$(cReset)"
	$(MUTE)$(CC) -c -mmcu=$(MCU) $< $(ALL_CFLAGS) -o $@
endif

# Include all auto-dependencies definitions
-include $(shell mkdir -p $(BUILD.DIR) 2>/dev/null) $(wildcard $(BUILD.DIR)/*.d)

else

#-----------------------------------------------------------------------------
# Scan for all valid .mak files
#-----------------------------------------------------------------------------
ALLMAKS:=$(foreach mak,\
   $(shell find $(MAKETOP) -type f -name '*.mak' -not -path '$(MAKETOP)/$(MAKEDIR)/*'),\
   $(if $(findstring $(notdir $(basename $(mak))),$(lastword $(subst /, ,$(dir $(abspath $(mak)))))),\
      $(mak),)\
)


#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# Expand the variables for a project
# $1 = project name
# $2 = project dir
# $3 = optional extended build path like sim/ or empty
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
define expandProjectVars
$1.DIR?=$2
$1.CFG?=$(DEFAULT.CFG)
$1.CSRCS=$$(foreach c, $$($1.C), $$($1.DIR)/$$(c).c) $$(foreach pick, $$($1.PICK), ./src/lib/$$(pick).c)
endef

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# First include the file
# $1 makfile
# $2 path
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
define includeAndExpandMak
override DIR=$2
include $2/$1.mak
endef

define makeHex
$(eval $(call expandProjectVars,$1,$(DIR)))
list::
	@$(ECHO) "$(cInfo). Hex target $(cVar)$1$(cInfo)$(cReset)"
hex::$1
$1 : $(DIST.DIR)/$1_$(BUILD.TYPE).hex
$(foreach cfile,$($1.CSRCS),$(eval $(call compileC,$1,$(cfile))))
.PHONY:$(DIST.DIR)/$1_$(BUILD.TYPE).hex
$(DIST.DIR)/$1_$(BUILD.TYPE).hex:
	$(MUTE)$(MAKE) -j$(SYSCPUS) --no-print-directory CFG=$$($1.CFG) SRC.DIR=$$($1.DIR) CSRCS="$$($1.CSRCS)" $$@
endef

define makeTest
$(eval $(subst Hex target,Test target, $(subst hex::,test::,$(makeHex))))
endef

define makeSim
$(eval $(call expandProjectVars,$1,$(DIR),sim/))
list::
	@$(ECHO) "$(cInfo). Sim target $(cVar)$1$(cInfo)$(cReset)"
sim::$1
$1 : $(DIST.DIR)/$1_$(BUILD.TYPE).exe
.PHONY:$(DIST.DIR)/$1_$(BUILD.TYPE).exe
$(DIST.DIR)/$1_$(BUILD.TYPE).exe :
	$(MUTE)$(MAKE) -j$(SYSCPUS) --no-print-directory CFG=$$($1.CFG) SRC.DIR=$$($1.DIR) BUILD.TYPE=sim CSRCS="$$($1.CSRCS)" $$@
endef

#-----------------------------------------------------------------------------
# Include all the mak files
#-----------------------------------------------------------------------------
$(foreach mak,$(ALLMAKS),$(eval $(call includeAndExpandMak,$(basename $(notdir $(mak))),$(subst /$$,,$(dir $(mak))$$))))

# First include
endif


#-------------------------------  End of File  -------------------------------

