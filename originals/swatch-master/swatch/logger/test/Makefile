SWATCH_ROOT= $(shell pwd)/../..

BUILD_HOME = $(SWATCH_ROOT)
include ${SWATCH_ROOT}/config/mfCommonDefs.mk

Project = swatch/test
Package = swatch/logger/test
PackagePath = $(SWATCH_ROOT)/logger/test
PackageName = cactuscore-swatch-logger-test

Packager = Alessandro Thea

PACKAGE_VER_MAJOR = 1
PACKAGE_VER_MINOR = 3
PACKAGE_VER_PATCH ?= 0
PACKAGE_RELEASE ?= 1

Library = cactus_swatch_logger_test
		  

Includes = 	\
	include \
	${SWATCH_ROOT}/logger/include \
	${SWATCH_ROOT}/logger/test/include \
	${CACTUS_ROOT}/include \
	${XDAQ_ROOT}/include

LibraryPaths =	\
	lib \
	${SWATCH_ROOT}/logger/lib \
	${CACTUS_ROOT}/lib \
	${XDAQ_ROOT}/lib

Libraries =	cactus_swatch_logger \
			  log4cplus \
			  boost_system \
			  boost_thread

ExecutableLibraries = ${Libraries} ${Library}

include ${SWATCH_ROOT}/config/mfRules.mk
include ${SWATCH_ROOT}/config/mfRPMRules.mk
