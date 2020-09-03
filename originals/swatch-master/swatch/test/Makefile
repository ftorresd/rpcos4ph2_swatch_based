SWATCH_ROOT= $(shell pwd)/..

BUILD_HOME = $(SWATCH_ROOT)
include ${SWATCH_ROOT}/config/mfCommonDefs.mk

Project = swatch/test
Package = swatch/test
PackagePath = $(SWATCH_ROOT)/test
PackageName = cactuscore-swatch-test

Packager = Alessandro Thea

PACKAGE_VER_MAJOR = 1
PACKAGE_VER_MINOR = 3
PACKAGE_VER_PATCH ?= 0
PACKAGE_RELEASE ?= 1

# Library = cactus_swatch_test

Includes = 	\
		include/ \
		${SWATCH_ROOT}/core/include \
		${SWATCH_ROOT}/action/include \
		${SWATCH_ROOT}/processor/include \
		${SWATCH_ROOT}/processor/test/include \
		${SWATCH_ROOT}/dtm/include \
		${SWATCH_ROOT}/dtm/test/include \
		${SWATCH_ROOT}/system/include \
		${SWATCH_ROOT}/system/test/include \
		${SWATCH_ROOT}/xml/include \
		${SWATCH_ROOT}/xml/test/include \
		${CACTUS_ROOT}/include \
		$(XDAQ_ROOT)/include \
		$(XDAQ_ROOT)/include/linux

LibraryPaths =	\
		lib \
		${SWATCH_ROOT}/logger/lib \
		${SWATCH_ROOT}/core/lib \
		${SWATCH_ROOT}/core/test/lib \
		${SWATCH_ROOT}/action/lib \
		${SWATCH_ROOT}/action/test/lib \
		${SWATCH_ROOT}/processor/lib \
		${SWATCH_ROOT}/processor/test/lib \
		${SWATCH_ROOT}/dtm/lib \
		${SWATCH_ROOT}/dtm/test/lib \
		${SWATCH_ROOT}/system/lib \
		${SWATCH_ROOT}/system/test/lib \
		${SWATCH_ROOT}/xml/lib \
		${SWATCH_ROOT}/xml/test/lib \
		${CACTUS_ROOT}/lib \
		$(XDAQ_ROOT)/lib \

Libraries =	\
		log4cplus \
		boost_system \
		boost_filesystem \
		boost_regex \
		boost_thread \
		boost_unit_test_framework \
		cactus_swatch_logger \
		cactus_swatch_core \
		cactus_swatch_core_test \
		cactus_swatch_action \
		cactus_swatch_action_test \
		cactus_swatch_processor \
		cactus_swatch_processor_test \
		cactus_swatch_dtm \
		cactus_swatch_dtm_test \
		cactus_swatch_system \
		cactus_swatch_system_test \
		cactus_swatch_xml \
		cactus_swatch_xml_test \
		dl

ExecutableLibraries = ${Libraries} ${Library}

include ${SWATCH_ROOT}/config/mfRules.mk
include ${SWATCH_ROOT}/config/mfRPMRules.mk

