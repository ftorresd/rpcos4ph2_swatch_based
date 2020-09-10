# RPCOS4Ph2 - SWATCH Based


## Launch the container

``docker run -p 3333:3333 -v `pwd`:/home/rpcos4ph2_dev_env -it ftorresd/swatch_build_env``


## Compile

Inside the container:

```
cd /home/rpcos4ph2_dev_env
source setup_env.sh
cd rpcos4ph2
make install
```

## Run

Inside the container:

```
cd /home/rpcos4ph2_dev_env
./runStandalone.sh 
```

The SWATCH Cell should be accesible at:
http://localhost:3333/urn:xdaq-application:lid=13/