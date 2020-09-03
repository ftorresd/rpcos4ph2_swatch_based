BUILD_HOME:=$(shell pwd)/..

include /opt/cactus/build-utils/mfCommonDefs.mk
include $(XDAQ_ROOT)/$(BUILD_SUPPORT)/mfAutoconf.rules
include $(XDAQ_ROOT)/$(BUILD_SUPPORT)/mfDefs.$(XDAQ_OS)

Project=rpcos4ph2
Package=dummy

Sources=\
	$(wildcard src/common/*.cpp)

IncludeDirs = \
	$(CACTUS_ROOT)/include \
	$(XDAQ_ROOT)/include


UserCCFlags = -g -std=c++11 -pipe


DependentLibraryDirs = \
	$(CACTUS_ROOT)/lib \
	$(XDAQ_ROOT)/lib

DependentLibraries = \
		cactus_mp7_mp7 \
		tsframework \
		tstoolbox \
		tsajaxell \
		cactus_swatch_core \
		cactus_swatch_system \
		cactus_swatch_processor \
		cactus_swatch_logger \
		cactus_amc13_amc13 \
		cactus_swatch_mp7 \
		log4cplus 
	
DynamicLibrary= rpcos4ph2_dummy


include $(XDAQ_ROOT)/$(BUILD_SUPPORT)/Makefile.rules
include $(XDAQ_ROOT)/$(BUILD_SUPPORT)/mfRPM.rules
