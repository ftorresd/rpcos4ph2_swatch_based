#!/bin/bash

#################################################
# 0. User Customization

SWATCH_DUMMY_INIT='${SWATCHEXAMPLE_ROOT}/test/dummySystem/dummySystem.xml'
SWATCH_DUMMY_GATEKEEPER_XML='${SWATCHEXAMPLE_ROOT}/test/dummySystem/config.xml'
SWATCH_DUMMY_GATEKEEPER_KEY='RunKey1'

SWATCH_HARDWARE_INIT='${SWATCHEXAMPLE_ROOT}/test/hardwareTest/hwSystem.xml'
SWATCH_HARDWARE_GATEKEEPER_XML='${SWATCHEXAMPLE_ROOT}/test/hardwareTest/config.xml'
SWATCH_HARDWARE_GATEKEEPER_KEY='RunKey1'

SWATCH_DEFAULT_INIT_FILE=${SWATCH_DUMMY_INIT}
SWATCH_DEFAULT_GATEKEEPER_XML=${SWATCH_DUMMY_GATEKEEPER_XML}
SWATCH_DEFAULT_GATEKEEPER_KEY=${SWATCH_DUMMY_GATEKEEPER_KEY}



#################################################
# 1. PARSE SCRIPT ARGUMENTS 

RUN_GDB=0
RUN_VALGRIND=0
LOAD_HW_MODS=0

SCRIPT_NAME=$(basename $0)
# read the options
OPTIND=1         # Reset in case getopts has been used previously in the shell.
TEMP=`getopt -o h,g --long help,hw,gdb,valgrind -n ${SCRIPT_NAME} -- "$@"`
if [ $? -ne 0 ]; then
    echo "ERROR : invalid argument given! Use '--help' option to discover correct arguments."
    exit 1
fi
eval set -- "$TEMP"

# extract options and their arguments into variables.
while true ; do
    case "$1" in
        -h|--help)
            echo "usage: ${SCRIPT_NAME} [-g|--gdb] [--valgrind] [--hw]"
            echo
            echo "Possible arguments ..."
            echo "  -g/--gdb     : Run cell within gdb"
            echo "  --valgrind   : Run cell within valgrind"
            echo "  --hw         : Load SWATCH hardware libraries, and use HW config files by default"
            exit 0 ;;
        -g|--gdb)
            echo "Running example cell in gdb";
            RUN_GDB=1;
            shift ;;
        --valgrind)
            echo "Running example cell in valgrind (memcheck)";
            RUN_VALGRIND=1;
            shift ;;
        --hw) 
            echo "Loading SWATCH hardware libraries";
            LOAD_HW_MODS=1;
            SWATCH_DEFAULT_INIT_FILE=${SWATCH_HARDWARE_INIT};
            SWATCH_DEFAULT_GATEKEEPER_XML=${SWATCH_HARDWARE_GATEKEEPER_XML}
            SWATCH_DEFAULT_GATEKEEPER_KEY=${SWATCH_HARDWARE_GATEKEEPER_KEY}
            shift ;;
        --)
            shift ;
            break ;;
        \?)
            echo "Internal error! Unknown option $1"
            exit 1 ;;
    esac
done

echo "Command line args parsed"


#################################################
# 2. SET ENVIRONMENT VARIABLES

export SWATCH_DEFAULT_INIT_FILE SWATCH_DEFAULT_GATEKEEPER_XML SWATCH_DEFAULT_GATEKEEPER_KEY

HERE=$(python -c "import os.path; print os.path.dirname(os.path.abspath('$BASH_SOURCE'))")

export SWATCH_ROOT=${HERE}/../../..
export LD_LIBRARY_PATH=${SWATCH_ROOT}/logger/lib:${SWATCH_ROOT}/core/lib:${SWATCH_ROOT}/action/lib:${SWATCH_ROOT}/xml/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=${SWATCH_ROOT}/processor/lib:${SWATCH_ROOT}/dtm/lib:${SWATCH_ROOT}/system/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=${SWATCH_ROOT}/dummy/lib:${SWATCH_ROOT}/amc13/lib:${SWATCH_ROOT}/mp7/lib:${LD_LIBRARY_PATH}:${SWATCH_ROOT}/mp7/test/lib:${LD_LIBRARY_PATH}

export SWATCHEXAMPLE_ROOT=${HERE}/..
export SWATCHFRAMEWORK_ROOT=${SWATCHEXAMPLE_ROOT}/../framework

export LD_LIBRARY_PATH=${SWATCHFRAMEWORK_ROOT}/lib:${LD_LIBRARY_PATH}
echo LD_LIBRARY_PATH=$LD_LIBRARY_PATH


export SUBSYSTEM_ID=dummySys
export SUBSYSTEM_CELL_LIB_PATH=${SWATCHEXAMPLE_ROOT}/lib/libswatchcellexample.so
export SUBSYSTEM_CELL_CLASS=swatchcellexample::Cell



##################################################
# 2. SET UP LINKS FOR LOCAL htdocs DIRECTORY

HTDOCS_LOCAL_ROOT=${HERE}/tmp_htdocs
export XDAQ_DOCUMENT_ROOT=${HTDOCS_LOCAL_ROOT}
echo XDAQ_DOCUMENT_ROOT=${XDAQ_DOCUMENT_ROOT}

mkdir -p ${HTDOCS_LOCAL_ROOT}

XDAQ_ROOT=/opt/xdaq
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



#################################################
# 3. Define SWATCH Additional libraries

if [[ "${LOAD_HW_MODS}" -eq 1 ]]; then
    SUBSYSTEM_CELL_LIB_PATH+=";/${SWATCH_ROOT}/mp7/lib/libcactus_swatch_mp7.so"
    SUBSYSTEM_CELL_LIB_PATH+=";/${SWATCH_ROOT}/mp7/test/lib/libcactus_swatch_mp7_test.so"
    SUBSYSTEM_CELL_LIB_PATH+=";/${SWATCH_ROOT}/amc13/lib/libcactus_swatch_amc13.so"
fi


#################################################
# 4. START THE CELL (interactively)

ARGS=""
if [[ "${RUN_GDB}" -eq 1 ]]; then
    ARGS+=" --gdb"
fi
if [[ "${RUN_VALGRIND}" -eq 1 ]]; then
    ARGS+=" --valgrind"
fi

${SWATCHFRAMEWORK_ROOT}/scripts/runSubsystemCell.sh $ARGS


