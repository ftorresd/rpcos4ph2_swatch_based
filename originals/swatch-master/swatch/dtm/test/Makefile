SWATCH_ROOT= $(shell pwd)/../..

BUILD_HOME = $(SWATCH_ROOT)
include ${SWATCH_ROOT}/config/mfCommonDefs.mk

Project = swatch/test
Package = swatch/dtm/test
PackagePath = $(SWATCH_ROOT)/dtm/test
PackageName = cactuscore-swatch-dtm-test

Packager = Alessandro Thea, Tom Williams

PACKAGE_VER_MAJOR = 1
PACKAGE_VER_MINOR = 3
PACKAGE_VER_PATCH ?= 0
PACKAGE_RELEASE ?= 1

Library = cactus_swatch_dtm_test

Includes = 	\
		include \
		${SWATCH_ROOT}/core/include \
		${SWATCH_ROOT}/action/include \
		${SWATCH_ROOT}/dtm/include \
		${CACTUS_ROOT}/include \
		$(XDAQ_ROOT)/include \
		$(XDAQ_ROOT)/include/linux

LibraryPaths =	\
		lib \
		${SWATCH_ROOT}/core/lib \
		${SWATCH_ROOT}/action/lib \
		${SWATCH_ROOT}/dtm/lib \
		${CACTUS_ROOT}/lib \
		$(XDAQ_ROOT)/lib


Libraries =	\
		boost_system \
		boost_unit_test_framework \
		cactus_swatch_core \
		cactus_swatch_action \
		dl


ExecutableLibraries = ${Libraries} ${Library}

include ${SWATCH_ROOT}/config/mfRules.mk
include ${SWATCH_ROOT}/config/mfRPMRules.mk
