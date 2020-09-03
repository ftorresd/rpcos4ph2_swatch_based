if [[ $_ == $0 ]]; then  
  echo "This script is meant to be sourced:"
  echo "  source $0"
  exit 1
fi

function pathadd() {
  PATH_NAME=${1:-}
  PATH_VAL=${!1:-""}
  PATH_ADD_VAL=${2:-}
  if [[ -z ${PATH_NAME} ]]; then
    >&2 echo "pathadd: varname not given"
  fi
  if [[ -z ${PATH_ADD_VAL} ]]; then
    >&2 echo "pathadd: value not given"
  fi

  if [[ ":$PATH_VAL:" == *":$PATH_ADD_VAL:"* ]]; then
    >&2 echo "warning: not adding duplicate '$PATH_ADD_VAL' in '$PATH_VAL'."
  else
    PATH_VAL="${PATH_ADD_VAL}${PATH_VAL:+":$PATH_VAL"}"
    echo "- ${PATH_NAME} += ${PATH_ADD_VAL}"

    # use eval to reset the target
    # eval "${PATH_NAME}=${PATH_VAL}"
    export ${PATH_NAME}=${PATH_VAL}
  fi
}

export CACTUS_ROOT=${CACTUS_ROOT:-"/opt/cactus"}
export XDAQ_ROOT=${XDAQ_ROOT:-"/opt/xdaq"}

pathadd LD_LIBRARY_PATH "${XDAQ_ROOT}/lib"
pathadd LD_LIBRARY_PATH "${CACTUS_ROOT}/lib"

export SWATCH_TESTS=$( readlink -f $(dirname $BASH_SOURCE)/ )
export SWATCH_ROOT=$( readlink -f ${SWATCH_TESTS}/.. )

pathadd PATH "${SWATCH_ROOT}/test/bin"

pathadd LD_LIBRARY_PATH "${SWATCH_ROOT}/logger/lib"
pathadd LD_LIBRARY_PATH "${SWATCH_ROOT}/core/lib"
pathadd LD_LIBRARY_PATH "${SWATCH_ROOT}/action/lib"
pathadd LD_LIBRARY_PATH "${SWATCH_ROOT}/processor/lib"
pathadd LD_LIBRARY_PATH "${SWATCH_ROOT}/dtm/lib"
pathadd LD_LIBRARY_PATH "${SWATCH_ROOT}/system/lib"
pathadd LD_LIBRARY_PATH "${SWATCH_ROOT}/xml/lib"
pathadd LD_LIBRARY_PATH "${SWATCH_ROOT}/test/lib"

# tests
pathadd LD_LIBRARY_PATH "${SWATCH_ROOT}/core/test/lib"
pathadd LD_LIBRARY_PATH "${SWATCH_ROOT}/action/test/lib"
pathadd LD_LIBRARY_PATH "${SWATCH_ROOT}/processor/test/lib"
pathadd LD_LIBRARY_PATH "${SWATCH_ROOT}/dtm/test/lib"
pathadd LD_LIBRARY_PATH "${SWATCH_ROOT}/system/test/lib"
pathadd LD_LIBRARY_PATH "${SWATCH_ROOT}/xml/test/lib"

# include boost test framework's messages of form Entering/leaving case/suite "blah"
export BOOST_TEST_LOG_LEVEL=test_suite

# export PATH PYTHONPATH LD_LIBRARY_PATH CACTUS_ROOT SWATCH_ROOT SWATCH_TESTS BOOST_TEST_LOG_LEVEL
