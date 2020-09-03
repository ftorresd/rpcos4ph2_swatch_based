#!/bin/bash

SWATCH_DEV_TOOLS=$( readlink -f $(dirname $BASH_SOURCE)/ )
SWATCH_ROOT=${SWATCH_DEV_TOOLS}/..

INCLUDE_PATHS="-I/opt/cactus/include -I/opt/xdaq/include -I/opt/xdaq/include/linux"
INCLUDE_PATHS="${INCLUDE_PATHS} -I${SWATCH_ROOT}/logger/include"
INCLUDE_PATHS="${INCLUDE_PATHS} -I${SWATCH_ROOT}/core/include"
INCLUDE_PATHS="${INCLUDE_PATHS} -I${SWATCH_ROOT}/processor/include"
INCLUDE_PATHS="${INCLUDE_PATHS} -I${SWATCH_ROOT}/system/include"
INCLUDE_PATHS="${INCLUDE_PATHS} -I${SWATCH_ROOT}/xml/include"
INCLUDE_PATHS="${INCLUDE_PATHS} -I${SWATCH_ROOT}/amc13/include"
INCLUDE_PATHS="${INCLUDE_PATHS} -I${SWATCH_ROOT}/mp7/include"

INCLUDE_PATHS="${INCLUDE_PATHS} -I${SWATCH_ROOT}/logger/test/include"
INCLUDE_PATHS="${INCLUDE_PATHS} -I${SWATCH_ROOT}/core/test/include"
INCLUDE_PATHS="${INCLUDE_PATHS} -I${SWATCH_ROOT}/processor/test/include"
INCLUDE_PATHS="${INCLUDE_PATHS} -I${SWATCH_ROOT}/system/test/include"
INCLUDE_PATHS="${INCLUDE_PATHS} -I${SWATCH_ROOT}/xml/test/include"
INCLUDE_PATHS="${INCLUDE_PATHS} -I${SWATCH_ROOT}/amc13/test/include"
INCLUDE_PATHS="${INCLUDE_PATHS} -I${SWATCH_ROOT}/mp7/test/include"


echo INCLUDE_PATHS=${INCLUDE_PATHS}
cppcheck ${INCLUDE_PATHS} --dump `find . -iname *.cpp`

