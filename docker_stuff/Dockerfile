FROM gitlab-registry.cern.ch/cms-cactus/core/cactus-buildenv/base-xdaq15:master-latest


COPY cactus-swatch.repo /etc/yum.repos.d/
COPY cactus-ts.repo /etc/yum.repos.d/


RUN yum clean all && \
    yum groupinstall -y triggersupervisor && \
    yum groupinstall -y swatch swatchcell && \
    yum install -y cactuscore-swatch-amc13 && \
    yum install -y cactuscore-swatchcellexample 

RUN yum -y install boost-devel pugixml-devel vim wget lynx zsh 

RUN wget https://raw.github.com/ohmyzsh/ohmyzsh/master/tools/install.sh -O - | zsh || true

ENTRYPOINT ["/usr/bin/scl", "enable", "devtoolset-8", "--", "/bin/bash", "-c"]
CMD ["/usr/bin/zsh"]