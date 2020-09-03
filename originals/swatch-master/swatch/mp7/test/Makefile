SWATCH_ROOT= $(shell pwd)/../..

BUILD_HOME = $(SWATCH_ROOT)
include ${SWATCH_ROOT}/config/mfCommonDefs.mk

Project = swatch/test
Package = swatch/mp7/test

PackagePath = $(SWATCH_ROOT)/mp7/test
PackageName = cactuscore-swatch-mp7-test

Packager = Alessandro Thea

PACKAGE_VER_MAJOR = 1
PACKAGE_VER_MINOR = 3
PACKAGE_VER_PATCH ?= 0
PACKAGE_RELEASE ?= 1

Library = cactus_swatch_mp7_test

Includes = \
	include \
	${SWATCH_ROOT}/core/include \
	${SWATCH_ROOT}/action/include \
	${SWATCH_ROOT}/processor/include \
	${SWATCH_ROOT}/mp7/include \
	${CACTUS_ROOT}/include \
	$(XDAQ_ROOT)/include \
	$(XDAQ_ROOT)/include/linux
	
LibraryPaths = \
	lib \
	${SWATCH_ROOT}/logger/lib \
	${SWATCH_ROOT}/core/lib \
	${SWATCH_ROOT}/action/lib \
	${SWATCH_ROOT}/processor/lib \
	${SWATCH_ROOT}/mp7/lib \
	${CACTUS_ROOT}/lib \
	$(XDAQ_ROOT)/lib

Libraries = \
	boost_unit_test_framework \
	cactus_uhal_uhal \
	cactus_mp7_mp7 \
	cactus_swatch_logger \
	cactus_swatch_core \
	cactus_swatch_action \
	cactus_swatch_processor \
	cactus_swatch_mp7

ExecutableLibraries = ${Libraries} ${Library}

include ${SWATCH_ROOT}/config/mfRules.mk
include ${SWATCH_ROOT}/config/mfRPMRules.mk
