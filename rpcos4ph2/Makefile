BUILD_HOME:=$(shell pwd)

Project=rpcos4ph2
Packages=dummy


# XDAQ makefiles want a pretty specific folder structure
# provide it using a symlink to free developers to put their git repos anywhere they fancy
# $(shell test ! -d $(Project) && mkdir -p `dirname $(Project)` && ln -s `pwd` $(Project))

include /opt/cactus/build-utils/mfCommonDefs.mk

include $(XDAQ_ROOT)/$(BUILD_SUPPORT)/mfAutoconf.rules
include $(XDAQ_ROOT)/$(BUILD_SUPPORT)/mfDefs.$(XDAQ_OS)

include $(XDAQ_ROOT)/$(BUILD_SUPPORT)/Makefile.rules
include $(XDAQ_ROOT)/$(BUILD_SUPPORT)/mfRPM.rules
