# RPCOS4Ph2 - SWATCH Based

## Dev env

### Login

`docker login gitlab-registry.cern.ch`


### Build

`docker build --tag ftorresd/swatch_build_env .`

### Run

`docker run -v `pwd`:/home/rpcos4ph2_dev_env -it ftorresd/swatch_build_env`

### Documentation

http://cactus.web.cern.ch/cactus/release/swatch/pipelines/gldirkx-xdaq15/usersGuide/install.html