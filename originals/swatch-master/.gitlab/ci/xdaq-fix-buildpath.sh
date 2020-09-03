#!/usr/bin/env bash
set -o errexit -o nounset -o pipefail
IFS=$'\n\t\v'
cd `dirname "${BASH_SOURCE[0]:-$0}"`/../..

# XDAQ makefiles still rely on the old SVN folder structure (repo/trunk/projects/cactuscore/project/subproject).
# The two major assumptions it makes are
# - the number of folders that are in $(pwd)
# - the length of $(pwd)
# This hack makes XDAQ-based makefiles build again without the SVN project structure

# beautiful
LONG_PATH="long________________________________________________________________/cactuscore"

# go to parent folder of 'swatch' (this used to be 'cactuscore')
cd ..
# create a 'cactuscore' folder, and a folder with a sufficiently long path
mkdir -p $LONG_PATH
# move the swatch folder there, symbolic link does not work
mv swatch $LONG_PATH
# put symbolic link on old location for your relative sanity
ln -s $LONG_PATH/swatch swatch
# go to new location
cd $LONG_PATH/swatch
