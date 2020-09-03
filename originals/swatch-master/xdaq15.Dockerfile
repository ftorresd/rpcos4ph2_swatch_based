ARG BASE_IMAGE=gitlab-registry.cern.ch/cms-cactus/core/ts/xdaq15-ts52:master-5c31872f

###

FROM golang:1.14-alpine as demo-cell-in-docker

COPY hacks /hacks
WORKDIR /hacks/demo-cell-in-docker
RUN CGO_ENABLED=0 go build ./cmd/wrap.go

###

FROM ${BASE_IMAGE}
LABEL maintainer="Cactus <cactus@cern.ch>"

COPY ci_rpms /rpms
RUN yum install -y /rpms/*.rpm && \
    yum clean all
COPY swatch /swatch
COPY --from=demo-cell-in-docker /hacks/demo-cell-in-docker/wrap /bin/xdaqwrapper

CMD ["xdaqwrapper /swatch/cell/example/test/runStandalone.sh"]
EXPOSE 80