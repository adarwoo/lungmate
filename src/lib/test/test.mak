#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# Unit tests for each API
#
# @author software@arreckx.com
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


#
# Dbg API unit test build configuration
#
testDbg.C=testDbg

$(eval $(call makeTest,testDbg))


#
# Uart API unit test build configuration
#
testUart.C=testUart
testUart.PICK=uart

$(eval $(call makeTest,testUart))


#
# nvParam API unit test build configuration
#
testParam.C=testParam
testParam.PICK=uart nvParam
testParam.CFG=nvParamTest

$(eval $(call makeTest,testParam))


#
# Console API unit test build configuration
#
testConsole.C=testConsole
testConsole.PICK=uart nvParam console

$(eval $(call makeTest,testConsole))


#
# adc API unit test build configuration
#
testAdc.C=testAdc
testAdc.PICK=uart adc
testAdc.CFG=adcTestCfg

$(eval $(call makeTest,testAdc))


#
# FFT API unit test build configuration
#
testFFT.C=testFFT
testFFT.PICK=uart fft

$(eval $(call makeTest,testFFT))


#
# Validate the nvParam in simulation
#
simParam.C=testParam
simParam.PICK=nvParam simUart simEeprom simAvr
simParam.CFG=$(testParam.CFG)

$(eval $(call makeSim,simParam))


#
# Validate the uart in simulation
#
simUart.C=testUart
simUart.PICK=simUart simAvr

$(eval $(call makeSim,simUart))


#
# Validate the console in simulation
#
simConsole.C=testConsole
simConsole.PICK=console nvParam simUart simEeprom simAvr
simConsole.CFG=$(testParam.CFG)

$(eval $(call makeSim,simConsole))


#
# Validate the FFT in simulation
#
simFFT.C=testFFT
simFFT.PICK=fft nvParam simUart simEeprom simAvr

$(eval $(call makeSim,simFFT))


#-------------------------------  End of File  -------------------------------

