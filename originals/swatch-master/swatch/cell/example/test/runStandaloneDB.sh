#!/bin/bash

#################################################
# 0. User Customization

SWATCH_DUMMY_INIT='${SWATCHEXAMPLE_ROOT}/test/dummySystem/dummy.json'
SWATCH_DUMMY_GATEKEEPER_XML='${SWATCHEXAMPLE_ROOT}/test/dummySystem/config.xml'
SWATCH_DUMMY_GATEKEEPER_KEY='RunKey1'

SWATCH_HARDWARE_INIT='${SWATCHEXAMPLE_ROOT}/test/hardwareTest/hardware.json'
SWATCH_HARDWARE_GATEKEEPER_XML='${SWATCHEXAMPLE_ROOT}/test/hardwareTest/config.xml'
SWATCH_HARDWARE_GATEKEEPER_KEY='RunKey1'

SWATCH_DEFAULT_INIT_FILE=${SWATCH_DUMMY_INIT}
SWATCH_DEFAULT_GATEKEEPER_XML=${SWATCH_DUMMY_GATEKEEPER_XML}
SWATCH_DEFAULT_GATEKEEPER_KEY=${SWATCH_DUMMY_GATEKEEPER_KEY}

#################################################
# 1. PARSE SCRIPT ARGUMENTS 

RUN_GDB=0
LOAD_HW_MODS=0

SCRIPT_NAME=$(basename $0)
# read the options
TEMP=`getopt -o g --long hw,gdb -n ${SCRIPT_NAME} -- "$@"`
eval set -- "$TEMP"

# extract options and their arguments into variables.
while true ; do
    case "$1" in
        -g|--gdb)
          echo "Running Example Cell in gdb";
          RUN_GDB=1;
          shift ;;
        --hw) 
          echo "Loading SWATCH Hardware libraries";
          LOAD_HW_MODS=1;
          SWATCH_DEFAULT_INIT_FILE=${SWATCH_HARDWARE_INIT};
          SWATCH_DEFAULT_GATEKEEPER_XML=${SWATCH_HARDWARE_GATEKEEPER_XML}
          SWATCH_DEFAULT_GATEKEEPER_KEY=${SWATCH_HARDWARE_GATEKEEPER_KEY}
          shift ;;
        --) shift ; break ;;
        *) echo "Internal error! Unknown option $1" ; exit 1 ;;
    esac
done

#################################################
# 2. SET ENVIRONMENT VARIABLES

# Check is cgdb is around
which cgdb > /dev/null
if [ $? -eq 0 ]; then
  GDB="cgdb";
else
  GDB="gdb";
fi

export SWATCH_DEFAULT_INIT_FILE SWATCH_DEFAULT_GATEKEEPER_XML SWATCH_DEFAULT_GATEKEEPER_KEY

export XDAQ_ROOT=/opt/xdaq

export LD_LIBRARY_PATH=${XDAQ_ROOT}/lib:/opt/cactus/lib:$LD_LIBRARY_PATH

HERE=$(python -c "import os.path; print os.path.dirname(os.path.abspath('$BASH_SOURCE'))")

export SWATCH_ROOT=${HERE}/../../../swatch
export LD_LIBRARY_PATH=${SWATCH_ROOT}/logger/lib:${SWATCH_ROOT}/core/lib:${SWATCH_ROOT}/xml/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=${SWATCH_ROOT}/processor/lib:${SWATCH_ROOT}/system/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=${SWATCH_ROOT}/dummy/lib:${SWATCH_ROOT}/amc13/lib:${SWATCH_ROOT}/mp7/lib:${LD_LIBRARY_PATH}:${SWATCH_ROOT}/mp7/test/lib:${LD_LIBRARY_PATH}

export SWATCHEXAMPLE_ROOT=${HERE}/..
export SWATCHFRAMEWORK_ROOT=${SWATCHEXAMPLE_ROOT}/../framework

export LD_LIBRARY_PATH=${SWATCHFRAMEWORK_ROOT}/lib/linux/x86_64_slc6:${LD_LIBRARY_PATH}

##################################################
# 2. SET UP LINKS FOR LOCAL htdocs DIRECTORY

HTDOCS_LOCAL_ROOT=${HERE}/tmp_htdocs
export XDAQ_DOCUMENT_ROOT=${HTDOCS_LOCAL_ROOT}
echo XDAQ_DOCUMENT_ROOT=${XDAQ_DOCUMENT_ROOT}

mkdir -p ${HTDOCS_LOCAL_ROOT}

HTDOCS_DIRS="${XDAQ_ROOT}/htdocs/executive ${XDAQ_ROOT}/htdocs/extern ${XDAQ_ROOT}/htdocs/hyperdaq ${XDAQ_ROOT}/htdocs/java"
HTDOCS_DIRS+=" ${XDAQ_ROOT}/htdocs/pt ${XDAQ_ROOT}/htdocs/ts ${XDAQ_ROOT}/htdocs/xgi ${XDAQ_ROOT}/htdocs/xrelay /tmp"

echo "Creating links to ${XDAQ_ROOT}/htdocs"
for DIR in ${HTDOCS_DIRS} ; do
  echo "   * ${DIR}"
  rm -f ${HTDOCS_LOCAL_ROOT}/`basename ${DIR}`
  ln -s ${DIR} ${HTDOCS_LOCAL_ROOT}/
done

echo "Creating links to htdocs directories within source code"
mkdir -p ${HTDOCS_LOCAL_ROOT}/swatchcell/framework
for SUBDIR in html images; do
  rm -f ${HTDOCS_LOCAL_ROOT}/swatchcell/framework/${SUBDIR}
  ln -s  ${SWATCHFRAMEWORK_ROOT}/${SUBDIR} ${HTDOCS_LOCAL_ROOT}/swatchcell/framework/
done

export PWD_PATH=/build/trunk/cactuscore/swatchcell/example/test
#################################################
# 3. Define SWATCH Additional libraries
# <xc:Module>file:///${SWATCHEXAMPLE_ROOT}/lib/linux/x86_64_slc6/libswatchcellexample.so</xc:Module>
SWATCH_EXTRA_LIBRARIES=()
SWATCH_EXTRA_LIBRARIES+=("file:///${SWATCHEXAMPLE_ROOT}/lib/linux/x86_64_slc6/libswatchcellexample.so")

if [[ "${LOAD_HW_MODS}" -eq 1 ]]; then
    SWATCH_EXTRA_LIBRARIES+=("file:///${SWATCH_ROOT}/mp7/lib/libcactus_swatch_mp7.so")
    SWATCH_EXTRA_LIBRARIES+=("file:///${SWATCH_ROOT}/mp7/test/lib/libcactus_swatch_mp7_test.so")
    SWATCH_EXTRA_LIBRARIES+=("file:///${SWATCH_ROOT}/amc13/lib/libcactus_swatch_amc13.so")
fi

# Build swatch module
SWATCH_MODULES=""
for SWLIB in ${SWATCH_EXTRA_LIBRARIES[@]}; do
    echo "Adding module : $SWLIB"
    SWATCH_MODULES="${SWATCH_MODULES}<xc:Module>${SWLIB}</xc:Module>"
done

#################################################
# 4. START THE CELL (interactively)

mkdir -p /tmp/swatchcell/
CONFIGURE_FILE=/tmp/swatchcell/standalone.configure

cp ${SWATCHEXAMPLE_ROOT}/test/standalone.configure $CONFIGURE_FILE
sed -i "s|__HOSTNAME__PORT__|`hostname`:2979|" $CONFIGURE_FILE
sed -i "s|__SWATCHCELL_MODULES__|${SWATCH_MODULES}|" $CONFIGURE_FILE


CMD="/opt/xdaq/bin/xdaq.exe -p 2979 -c ${CONFIGURE_FILE}"
if [[ "${RUN_GDB}" -eq 1 ]]; then
    ${GDB} --args $CMD
else
    $CMD
fi


