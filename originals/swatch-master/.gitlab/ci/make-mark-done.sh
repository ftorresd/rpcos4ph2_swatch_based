#!/usr/bin/env bash
set -o errexit -o nounset -o pipefail
IFS=$'\n\t\v'
cd `dirname "${BASH_SOURCE[0]:-$0}"`/../..

find -name '*.exe' -exec touch {} +
find -name '*.so' -exec touch {} +
find -name '*.o' -exec touch {} +
find -name '*.d' -exec touch {} +