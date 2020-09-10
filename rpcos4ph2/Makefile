BUILD_HOME:=$(shell pwd)

Project = rpcos4ph2

Packages = \
		dummy \
		cell 

# trick to avoid awk complaining about the version.h
$(shell ln -s /home/rpcos4ph2_dev_env/rpcos4ph2/dummy/include/rpcos4ph2/dummy /home/rpcos4ph2_dev_env/rpcos4ph2/dummy/include/dummy)

include $(CACTUS_ROOT)/build-utils/mfCommonDefs.mk

include $(XDAQ_ROOT)/$(BUILD_SUPPORT)/mfAutoconf.rules
include $(XDAQ_ROOT)/$(BUILD_SUPPORT)/mfDefs.$(XDAQ_OS)

include $(XDAQ_ROOT)/$(BUILD_SUPPORT)/Makefile.rules
include $(XDAQ_ROOT)/$(BUILD_SUPPORT)/mfRPM.rules
