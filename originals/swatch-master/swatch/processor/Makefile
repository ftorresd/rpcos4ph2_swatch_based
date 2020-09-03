SWATCH_ROOT= $(shell pwd)/..

BUILD_HOME = $(SWATCH_ROOT)
include ${SWATCH_ROOT}/config/mfCommonDefs.mk

Project = swatch
Package = swatch/logger

PackagePath = $(SWATCH_ROOT)/processor
PackageName = cactuscore-swatch-processor

Packager = Alessandro Thea

PACKAGE_VER_MAJOR = 1
PACKAGE_VER_MINOR = 3
PACKAGE_VER_PATCH ?= 0
PACKAGE_RELEASE ?= 1

Library = cactus_swatch_processor


Includes =	\
		include  \
		${SWATCH_ROOT}/core/include \
		${SWATCH_ROOT}/action/include \
		${CACTUS_ROOT}/include \
		$(XDAQ_ROOT)/include \
		$(XDAQ_ROOT)/include/linux

LibraryPaths =  \
	lib \
	${SWATCH_ROOT}/core/lib \
	${SWATCH_ROOT}/action/lib \
	${CACTUS_ROOT}/lib \
	$(XDAQ_ROOT)/lib \ 
	
Libraries =	\
		cactus_swatch_core

include ${SWATCH_ROOT}/config/mfRules.mk
include ${SWATCH_ROOT}/config/mfRPMRules.mk
