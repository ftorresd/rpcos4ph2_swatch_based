#!/bin/bash


##################################################
# 0. CHECK THAT REQUIRED ENV VARIABLES ARE DEFINED

if [ -z "$SWATCH_DEFAULT_INIT_FILE" ]; then
    echo "Need to set SWATCH_DEFAULT_INIT_FILE!"; exit 1
fi

if [ -z "$SWATCH_DEFAULT_INIT_FILE" ]; then
    echo "Need to set SWATCH_DEFAULT_GATEKEEPER_XML!"; exit 1
fi

if [ -z "$SWATCH_DEFAULT_GATEKEEPER_KEY" ]; then
    echo "Need to set SWATCH_DEFAULT_GATEKEEPER_KEY!"; exit 1
fi

if [ -z "$SUBSYSTEM_ID" ]; then
    echo "Need to set SUBSYSTEM_ID!"; exit 1
fi

if [ -z "$SUBSYSTEM_CELL_LIB_PATH" ]; then
    echo "Need to set SUBSYSTEM_CELL_LIB_PATH!"; exit 1
fi

if [ -z "$SUBSYSTEM_CELL_CLASS" ]; then
    echo "Need to set SUBSYSTEM_CELL_CLASS!"; exit 1
fi


#################################################
# 1. PARSE SCRIPT ARGUMENTS

# A POSIX variable
OPTIND=1         # Reset in case getopts has been used previously in the shell.

RUN_GDB=0
RUN_VALGRIND=0

SCRIPT_NAME=$(basename $0)
TEMP=`getopt -o g --long help,gdb,valgrind -n ${SCRIPT_NAME} -- "$@"`
if [ $? -ne 0 ]; then
    echo "ERROR : invalid option given! Use '--help' option to discover correct arguments."
    exit 1
fi
eval set -- "$TEMP"

while true ; do
    case "$1" in
        -h|--help)
            echo "usage: $(basename $0) [-g|--gdb] [--valgrind]"
            echo
            echo "Possible arguments ..."
            echo "  -g/--gdb     : Run cell within gdb"
            echo "  --valgrind   : Run cell within valgrind"
            exit 0 ;;
        -g|--gdb)
            echo "-g/--gdb was triggered, Parameter: $OPTARG" >&2
            RUN_GDB=1
            shift ;;
        --valgrind)  echo "-v was triggered, Parameter: $OPTARG"
            RUN_VALGRIND=1
            shift ;;
        --)
            shift ;
            break ;;
        \?)
            echo "Invalid option: -$OPTARG" >&2
            exit 1
            ;;
    esac
done



##################################################
# 2. SET COMMON ENVIRONMENT VARIABLES

export XDAQ_ROOT=/opt/xdaq
export XDAQ_DOCUMENT_ROOT=${XDAQ_DOCUMENT_ROOT:-/opt/xdaq/htdocs}

export LD_LIBRARY_PATH=/opt/xdaq/lib:/opt/cactus/lib:${LD_LIBRARY_PATH}



#################################################
# 3. CREATE MODIFIED CONFIG FILES UNDER /tmp  (AS NEEDED)

TMP_RUNCELL_DIR=/tmp/${SUBSYSTEM_ID}cell
mkdir -p ${TMP_RUNCELL_DIR}
CONFIGURE_FILE=${TMP_RUNCELL_DIR}/standalone.configure
echo "Creating config file: ${CONFIGURE_FILE}"

HERE=$(python -c "import os.path; print os.path.dirname(os.path.abspath('$BASH_SOURCE'))")
if [ "$(basename ${HERE})" == "scripts" ]; then
  # Being run from checked out source code
  SWATCHCELL_ETC_DIR=${HERE}/../etc/swatchcell
else
  # Being run from installed RPMs
  SWATCHCELL_ETC_DIR=${HERE}/../../etc/swatchcell
fi
echo "  SWATCHCELL_ETC_DIR=${SWATCHCELL_ETC_DIR}"

MODULE_TAGS=""

for i_LIB_PATH in $(echo "${SUBSYSTEM_CELL_LIB_PATH}" | tr ";" "\n")
do
    echo "Adding module : $i_LIB_PATH"
    MODULE_TAGS="${MODULE_TAGS}<xc:Module>${i_LIB_PATH}</xc:Module>"
done

sed "s|__HOSTNAME__PORT__|`hostname`:3333|" ${SWATCHCELL_ETC_DIR}/common.configure > $CONFIGURE_FILE
sed -i "s|__SUBSYSTEM_CELL_CLASS__|${SUBSYSTEM_CELL_CLASS}|" ${CONFIGURE_FILE}
sed -i "s|__SUBSYSTEM_ID__|${SUBSYSTEM_ID}|" ${CONFIGURE_FILE}
sed -i "s|__MODULES_TO_LOAD__|${MODULE_TAGS}|" ${CONFIGURE_FILE}


export SWATCH_LOG4CPLUS_CONFIG=${TMP_RUNCELL_DIR}/log4cplus.properties
sed "s|__LOGFILE__|${TMP_RUNCELL_DIR}/swatch.log|" ${SWATCHCELL_ETC_DIR}/log4cplus.properties  >  ${SWATCH_LOG4CPLUS_CONFIG}
sed -i "s|__VERBOSE_LOGFILE__|${TMP_RUNCELL_DIR}/swatch_verbose.log|" ${SWATCH_LOG4CPLUS_CONFIG}
sed -i "s|__SUBSYSTEM_ID__|${SUBSYSTEM_ID}|" ${SWATCH_LOG4CPLUS_CONFIG}

#################################################
# 4. START THE CELL (interactively)

PROFILE_FILE=${SWATCHCELL_ETC_DIR}/common.profile
CMD="/opt/xdaq/bin/xdaq.exe -p 3333 -c ${CONFIGURE_FILE} -e ${PROFILE_FILE}"

VALGRIND_ARGS="--tool=memcheck --error-exitcode=1 --leak-check=full --num-callers=100"
#VALGRIND_ARGS="--tool=massif --max-snapshots=1000"
#VALGRIND_ARGS="--tool=exp-sgcheck"
if [[ "${RUN_GDB}" -eq 1 ]]; then
    VALGRIND_ARGS+=" --vgdb=yes --vgdb-error=0"
fi

if [[ "${RUN_VALGRIND}" -eq 1 ]]; then
    echo "Running command: valgrind $VALGRIND_ARGS $CMD"
    valgrind $VALGRIND_ARGS $CMD
elif [[ "${RUN_GDB}" -eq 1 ]]; then
    gdb --args $CMD
else
    # Enable core dumps
    ulimit -S -c unlimited
    # Go!
    $CMD
fi

