SWATCH
======

SWATCH (<b>S</b>oft<b>W</b>are for <b>A</b>utomating con<b>T</b>rol of <b>C</b>ommon <b>H</b>ardware) is a software framework that is used to control and monitor the phase-1 upgrade of the CMS level-1 trigger system.

Much of the information in this README is meant for developers of the SWATCH libraries. Subsystem developers should instead read the user's guide which can be found here: http://cactus.web.cern.ch/cactus/release/swatch/latest_doc/html/index.html


# Notes for developers of SWATCH

## Run the unit tests 

The tests are based on the boost unit test framework. Before running the unit tests, you can setup
your `LD_LIBRARY_PATH` using `test/env.sh` as follows:
~~~
source swatch/test/env.sh
~~~
This script also sets the `BOOST_TEST_LOG_LEVEL` environment variable, so that the boost unit test 
framework prints to the console at the start and end of each test suite/case. 

You can run tests either for all of swatch, per subsystem or individually
 - all:
~~~
./swatch/test/bin/boostTest.exe
~~~

 - individual:
~~~
 ./test/bin/boostTest.exe --run_test=SystemTestSuite/BuildSystem
~~~

You can change the log threshold for the boost unit test framework using the `--log_level` 
command-line option; e.g. to display all messages (including each time that a check passes):
~~~
./test/bin/boostTest --log_level=all
~~~


## Run the example SWATCH cell

The scripts and configuration files for running the example SWATCH cell can be found under `swatch/cell/example/test`

The example SWATCH cell can be run as follows:

~~~
./swatch/cell/test/runStandalone.sh
~~~

This script accepts a few options (that can be listed by adding the `--help` argument):

 * `--gdb` : Run cell under gdb
 * `--valgrind` : Run cell under valgrind
 * `--hw` : Loads SWATCH plugin libraries for AMC13 & MP7 

