SWATCH_ROOT = $(shell pwd)/../..
BUILD_HOME:=$(shell pwd)/..

ifndef PROJECT_NAME
PROJECT_NAME=rpcos4ph2
endif

include $(CACTUS_ROOT)/build-utils/mfCommonDefs.mk
include $(XDAQ_ROOT)/$(BUILD_SUPPORT)/mfAutoconf.rules
include $(XDAQ_ROOT)/$(BUILD_SUPPORT)/mfDefs.$(XDAQ_OS)
#
# Package to be built
#
Project=$(PROJECT_NAME)
Package=cell

#
# Source files
#
Sources=$(wildcard src/common/*.cpp)

#
# Include directories
#
IncludeDirs = \
	$(CACTUS_ROOT)/include \
	$(XDAQ_ROOT)/include \
	$(BUILD_HOME)/dummy/include

UserCCFlags = -g -std=c++11 -pipe

	
DependentLibraryDirs = \
	$(CACTUS_ROOT)/lib \
	$(XDAQ_ROOT)/lib \
	$(BUILD_HOME)/dummy/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) 



DependentLibraries = \
	tsframework \
	tstoolbox \
	tsajaxell \
	log4cplus \
	cactus_uhal_log \
	cactus_swatch_core \
	cactus_swatch_action \
	cactus_swatch_processor \
	cactus_swatch_dtm \
	cactus_swatch_system \
	rpcos4ph2_dummy \
	swatchcellframework

# LibraryDirs = $(XDAQ_ROOT)/lib

#
# Compile the source files and create a shared library
#
DynamicLibrary = rpcos4ph2_cell

include $(XDAQ_ROOT)/$(BUILD_SUPPORT)/Makefile.rules
include $(XDAQ_ROOT)/$(BUILD_SUPPORT)/mfRPM.rules





