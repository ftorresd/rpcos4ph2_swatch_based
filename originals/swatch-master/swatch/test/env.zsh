SCRIPT_SOURCE=$_

CACTUS_ROOT="/opt/cactus"
SWATCH_TESTS=$( readlink -f $(dirname $SCRIPT_SOURCE)/ )
SWATCH_ROOT=$( readlink -f ${SWATCH_TESTS}/.. )

# tie LD_LIBRARY_PATH into an array
typeset -T LD_LIBRARY_PATH ld_library_path

path+=("${SWATCH_ROOT}/test/bin")
#ld_library_path+=("${SWATCH_ROOT}/tests/src/python")
ld_library_path+=("${SWATCH_ROOT}/core/lib")
ld_library_path+=("${SWATCH_ROOT}/processor/lib")
ld_library_path+=("${SWATCH_ROOT}/system/lib")
ld_library_path+=("${SWATCH_ROOT}/test/lib")



ld_library_path+=("${SWATCH_ROOT}/core/test/lib")
ld_library_path+=("${SWATCH_ROOT}/processor/test/lib")
ld_library_path+=("${SWATCH_ROOT}/system/test/lib")

typeset -U PATH
typeset -U LD_LIBRARY_PATH

export PATH LD_LIBRARY_PATH CACTUS_ROOT SWATCH_ROOT SWATCH_TESTS

