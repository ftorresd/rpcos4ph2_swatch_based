SWATCH_ROOT= $(shell pwd)/..

BUILD_HOME = $(SWATCH_ROOT)
include ${SWATCH_ROOT}/config/mfCommonDefs.mk

Project = swatch
Package = swatch/logger

PackagePath = $(SWATCH_ROOT)/logger
PackageName = cactuscore-swatch-logger

Packager = Alessandro Thea

PACKAGE_VER_MAJOR = 1
PACKAGE_VER_MINOR = 3
PACKAGE_VER_PATCH ?= 0
PACKAGE_RELEASE ?= 1

Library = cactus_swatch_logger

Includes = \
	include \
	${CACTUS_ROOT}/include \
	${XDAQ_ROOT}/include
	
LibraryPaths = \
	lib \
	${XDAQ_ROOT}/lib

Libraries = log4cplus

include ${SWATCH_ROOT}/config/mfRules.mk
include ${SWATCH_ROOT}/config/mfRPMRules.mk
