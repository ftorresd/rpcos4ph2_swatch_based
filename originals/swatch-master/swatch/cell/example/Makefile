SWATCH_ROOT = $(shell pwd)/../..

BUILD_HOME = $(SWATCH_ROOT)
include $(SWATCH_ROOT)/config/mfCommonDefs.mk

Project = swatchcell
Package = swatch/cell/example

PackagePath = $(SWATCH_ROOT)/cell/example
PackageName = cactuscore-swatchcellexample

Packager = Alessandro Thea, Tom Williams

PACKAGE_VER_MAJOR = 1
PACKAGE_VER_MINOR = 3
PACKAGE_VER_PATCH ?= 0
PACKAGE_RELEASE ?= 1

Library = swatchcellexample


Includes = \
	include \
	$(SWATCH_ROOT)/cell/framework/include \
	$(CACTUS_ROOT)/include \
	$(XDAQ_ROOT)/include \
	$(XDAQ_ROOT)/include/linux \
	$(SWATCH_ROOT)/core/include \
	$(SWATCH_ROOT)/action/include \
	$(SWATCH_ROOT)/processor/include \
	$(SWATCH_ROOT)/dtm/include \
	$(SWATCH_ROOT)/system/include

CxxFlags += -ggdb

LibraryPaths = \
	$(SWATCH_ROOT)/cell/framework/lib \
	$(CACTUS_ROOT)/lib \
	$(XDAQ_ROOT)/lib \
	$(SWATCH_ROOT)/core/lib \
	$(SWATCH_ROOT)/action/lib \
	$(SWATCH_ROOT)/processor/lib \
	$(SWATCH_ROOT)/dtm/lib \
	$(SWATCH_ROOT)/system/lib \
	$(SWATCH_ROOT)/dummy/lib \
	$(SWATCH_ROOT)/amc13/lib \
	$(SWATCH_ROOT)/mp7/lib


Libraries = \
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
	cactus_swatch_dummy \
	swatchcellframework



include $(SWATCH_ROOT)/config/mfRules.mk
include $(SWATCH_ROOT)/config/mfRPMRules.mk


