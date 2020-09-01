FROM gitlab-registry.cern.ch/cms-cactus/core/cactus-buildenv/base-xdaq15:master-latest


COPY cactus-swatch.repo /etc/yum.repos.d/
COPY cactus-ts.repo /etc/yum.repos.d/


RUN yum clean all && \
    yum groupinstall -y triggersupervisor && \
    yum groupinstall -y swatch swatchcell && \
    yum install -y cactuscore-swatch-amc13 

RUN yum -y install boost-devel pugixml-devel

ENTRYPOINT ["/usr/bin/scl", "enable", "devtoolset-8", "--", "/bin/bash", "-c"]
CMD ["/bin/bash"]