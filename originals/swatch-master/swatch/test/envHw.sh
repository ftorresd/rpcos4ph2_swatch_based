if [[ $_ == $0 ]]; then  
  echo "This script is meant to be sourced:"
  echo "  source $0"
  exit 0
fi

#
function pathadd() {
  # TODO add check for empty path
  # and what happens if $1 == $2
  # Copy into temp variables
  PATH_NAME=$1
  PATH_VAL=${!1}
  if [[ ":$PATH_VAL:" != *":$2:"* ]]; then
    PATH_VAL="$2${PATH_VAL:+":$PATH_VAL"}"
    echo "- $1 += $2"

    # use eval to reset the target
    eval "$PATH_NAME=$PATH_VAL"
  fi
}

source $( readlink -f $(dirname $BASH_SOURCE)/env.sh )

pathadd PATH "${SWATCH_ROOT}/mp7/test/bin"

pathadd LD_LIBRARY_PATH "${SWATCH_ROOT}/mp7/lib"

# tests
pathadd LD_LIBRARY_PATH "${SWATCH_ROOT}/mp7/test/lib"

# include boost test framework's messages of form Entering/leaving case/suite "blah"
BOOST_TEST_LOG_LEVEL=test_suite

export PATH LD_LIBRARY_PATH