SWATCH_ROOT = $(shell pwd)/../..

BUILD_HOME = $(SWATCH_ROOT)
include ${SWATCH_ROOT}/config/mfCommonDefs.mk

Project = swatchcell
Package = swatch/cell/framework

PackagePath = $(SWATCH_ROOT)/cell/framework
PackageName = cactuscore-swatchcellframework

Packager = Alessandro Thea, Tom Williams

PACKAGE_VER_MAJOR = 1
PACKAGE_VER_MINOR = 3
PACKAGE_VER_PATCH ?= 0
PACKAGE_RELEASE ?= 1

Library = swatchcellframework


Includes = \
	include \
	$(CACTUS_ROOT)/include \
	$(XDAQ_ROOT)/include \
	$(XDAQ_ROOT)/include/linux \
	$(SWATCH_ROOT)/logger/include \
	$(SWATCH_ROOT)/core/include \
	$(SWATCH_ROOT)/action/include \
	$(SWATCH_ROOT)/processor/include \
	$(SWATCH_ROOT)/dtm/include \
	$(SWATCH_ROOT)/system/include \
	$(SWATCH_ROOT)/xml/include

CxxFlags += -ggdb

LibraryPaths = \
    $(SWATCH_ROOT)/logger/lib \
	$(SWATCH_ROOT)/core/lib \
	$(SWATCH_ROOT)/action/lib \
	$(SWATCH_ROOT)/processor/lib \
	$(SWATCH_ROOT)/dtm/lib \
	$(SWATCH_ROOT)/system/lib \
	$(SWATCH_ROOT)/xml/lib \
	$(CACTUS_ROOT)/lib \
	$(XDAQ_ROOT)/lib

Libraries = \
	tsframework \
	tsruncontrol \
	tstoolbox \
	tsajaxell \
	log4cplus \
	boost_chrono \
	cactus_uhal_log \
	cactus_swatch_logger \
	cactus_swatch_core \
	cactus_swatch_action \
	cactus_swatch_system \
	cactus_swatch_xml \
	tscandela

HtDocsPath = $(XDAQ_ROOT)/htdocs/swatchcell/framework



include ${SWATCH_ROOT}/config/mfRules.mk
include ${SWATCH_ROOT}/config/mfRPMRules.mk
