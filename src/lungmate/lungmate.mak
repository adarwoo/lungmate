#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# Make rules for building the lungmate and a board test software
# This mak file will add the targets
#  lungmate  ... builds the lungmate software (part of the hex targets)
#  testBoard ... builds the board test application (part of the test targets)
#
# @author software@arreckx.com
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#
# Build the lungmate hex file
#
lungmate.C=preamble lungmate stateMachine fusesAndLockBits
lungmate.PICK=uart console adc fft key nvParam

$(eval $(call makeHex,lungmate))


#
# Build the board test software
#
boardTest.C=boardTest
boardTest.PICK=adc uart nvParam fft key

$(eval $(call makeTest,boardTest))


#-------------------------------  End of File  -------------------------------

