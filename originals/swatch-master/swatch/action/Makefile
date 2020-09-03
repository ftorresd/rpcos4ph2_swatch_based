SWATCH_ROOT= $(shell pwd)/..

BUILD_HOME = $(SWATCH_ROOT)
include ${SWATCH_ROOT}/config/mfCommonDefs.mk

Project = swatch
Package = swatch/action

PackagePath = $(SWATCH_ROOT)/action
PackageName = cactuscore-swatch-action

Packager = Alessandro Thea

PACKAGE_VER_MAJOR = 1
PACKAGE_VER_MINOR = 3
PACKAGE_VER_PATCH ?= 0
PACKAGE_RELEASE ?= 1

Library = cactus_swatch_action

Includes = \
	include \
	${SWATCH_ROOT}/logger/include \
	${SWATCH_ROOT}/core/include \
	${CACTUS_ROOT}/include \
	$(XDAQ_ROOT)/include \
	$(XDAQ_ROOT)/include/linux

LibraryPaths = lib \
	$(XDAQ_ROOT)/lib \
	$(CACTUS_ROOT)/lib \
	${SWATCH_ROOT}/logger/lib \
	${SWATCH_ROOT}/core/lib \
	/lib64

Libraries =	\
	log4cplus \
	uuid \
	config \
	xerces-c \
	toolbox \
	xcept \
	xdata \
	asyncresolv \
	mimetic \
	xoap \
	boost_regex \
	pthread \
	boost_thread \
	boost_chrono \
	cactus_swatch_logger \
	cactus_swatch_core
	

include ${SWATCH_ROOT}/config/mfRules.mk
include ${SWATCH_ROOT}/config/mfRPMRules.mk
