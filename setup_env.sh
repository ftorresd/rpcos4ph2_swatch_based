export XDAQ_ROOT=/opt/xdaq
export BUILD_SUPPORT=build 
export XDAQ_DOCUMENT_ROOT=${XDAQ_ROOT}/htdocs
export CACTUS_ROOT=/opt/cactus
# export ROOTSYS=/opt/root/hcal-root-5.28.00f-gcc412
# export LD_LIBRARY_PATH=${ROOTSYS}/lib:${XDAQ_ROOT}/lib:${CACTUS_ROOT}/lib
export RPCOS4PH2_LIBRARY_PATH=`pwd`/rpcos4ph2/x86_64_centos7/lib
# export LD_LIBRARY_PATH=${XDAQ_ROOT}/lib:${CACTUS_ROOT}/lib
export LD_LIBRARY_PATH=${XDAQ_ROOT}/lib:${CACTUS_ROOT}/lib:${RPCOS4PH2_LIBRARY_PATH}

# export XDAQ_OS=linux
# export PATH=${PATH}:${XDAQ_ROOT}/bin:${ROOTSYS}/bin:/nfshome0/hcalcfg/bin
# export PYTHONPATH=${ROOTSYS}/lib:${PYTHONPATH}

# export AMC13_SERIAL_NO=10


# SUBSYSTEM_ID : The ID of your subsystem (e.g. calol1 , calol2 , ugt , …); must not contain spaces.
# SUBSYSTEM_CELL_CLASS : Fully-qualified name of your Cell class (e.g. calol2::cell::Cell )
# SUBSYSTEM_CELL_LIB_PATH : Path to the library containing your Cell class (multiple libraries can be specified using a semi-colon as delimeter ; )
# SWATCH_DEFAULT_INIT_FILE : path for system description JSON file
# SWATCH_DEFAULT_GATEKEEPER_XML : path for XML configuration file
# SWATCH_DEFAULT_GATEKEEPER_KEY : run key for XML configuration file

# HERE=$(python -c "import os.path; print os.path.dirname(os.path.abspath('$BASH_SOURCE'))")
# export RPCOS4PH2_ROOT=${HERE}

# export PWD_PATH=/home/rpcos4ph2_dev_env/rpcos4ph2/config/
export SUBSYSTEM_ID=rpcos4ph2
export SUBSYSTEM_CELL_CLASS=rpcos4ph2::cell::Cell 
export SUBSYSTEM_CELL_LIB_PATH=/home/rpcos4ph2_dev_env/rpcos4ph2/x86_64_centos7/lib/librpcos4ph2_cell.so
# export SUBSYSTEM_CELL_LIB_PATH=${RPCOS4PH2_LIBRARY_PATH}/librpcos4ph2_cell.so;${RPCOS4PH2_LIBRARY_PATH}/librpcos4ph2_dummy.so
export SWATCH_DEFAULT_INIT_FILE=/home/rpcos4ph2_dev_env/rpcos4ph2/config/dummySystem.xml
# export SWATCH_DEFAULT_GATEKEEPER_XML=${RPCOS4PH2_ROOT}/rpcos4ph2/etc/dummySystem/config.xml
export SWATCH_DEFAULT_GATEKEEPER_KEY=RunKey1

# export PWD_PATH=${RPCOS4PH2_ROOT}/rpcos4ph2/etc

# sudo ln -s $RPCOS4PH2_ROOT/rpcos4ph2/supervisor/cell/html $XDAQ_ROOT/htdocs/rpcos4ph2 

echo "Start the RPCOS4Ph2 Cell:"
echo "./runSubsystemCell.sh"

# source setup_env.sh ; ./runSubsystemCell.sh