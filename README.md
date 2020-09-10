# RPCOS4Ph2 - SWATCH Based

## Compile

Inside the Docker container:

```
cd /home/rpcos4ph2_dev_env/rpcos4ph2
make install -j`nproc`
```

## Launch the container

``docker run -p 3333:3333 -v `pwd`:/home/rpcos4ph2_dev_env -it ftorresd/swatch_build_env``


## Run

```
cd /home/rpcos4ph2_dev_env/rpcos4ph2
source setup_env.sh
/opt/cactus/bin/swatchcell/runSubsystemCell.sh 
```

The SWATCH Cell should be accesible at:
http://localhost:3333/urn:xdaq-application:lid=13/