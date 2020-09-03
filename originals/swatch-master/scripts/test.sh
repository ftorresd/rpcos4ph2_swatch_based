#!/usr/bin/env bash
set -o errexit -o nounset -o pipefail
IFS=$'\n\t\v'
cd `dirname "${BASH_SOURCE[0]:-$0}"`/../swatch

TARGET=${1:-"all"}

source test/env.sh
if [[ "$TARGET" == "unit" || "$TARGET" == "all" ]];then
  test/bin/boostTest.exe
fi
if [[ "$TARGET" == "valgrind" || "$TARGET" == "all" ]];then
  valgrind --error-exitcode=1 --tool=memcheck --leak-check=full --suppressions=test/swatch.supp test/bin/boostTest.exe 2>&1 | sed -e "s/ERROR SUMMARY: 0 errors/SUMMARY: 0 errors/g"
fi

if [[ "$TARGET" == "mp7" || "$TARGET" == "all" ]];then
  source mp7/test/env.sh
  mp7/test/bin/mp7Tester.exe
fi