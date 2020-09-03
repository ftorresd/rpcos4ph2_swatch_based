#!/usr/bin/env bash
set -o errexit -o nounset -o pipefail
IFS=$'\n\t\v'
cd `dirname "${BASH_SOURCE[0]:-$0}"`/../..

SSH_USER=${SSH_USER:-`whoami`}
BASTION_HOSTS=${BASTION_HOSTS:-$SSH_USER@lxplus.cern.ch,$SSH_USER@cmsusr.cern.ch}

rm -rf p5_rpms && mkdir -p p5_rpms
find . -name "*.rpm" ! -path "./p5_rpms/*" -exec cp -n -t p5_rpms/ "{}" +

ls -l p5_rpms
echo "uploading RPMs to drobox host"
scp -ro "ProxyJump $BASTION_HOSTS" p5_rpms $SSH_USER@cmsdropbox.cms:~

echo "triggering release on dropbox host"
ssh -J "$BASTION_HOSTS" $SSH_USER@cmsdropbox.cms sudo dropbox2 -o cc7 -z cms -s trigger -u p5_rpms && rm -rf p5_rpms

# from Tom
# sudo dropbox2 -o cc7 -z cms -s trigger -u /nfshome0/triggerdev/rpms/ts/v5.1.2 

# ssh jumphost example
# ssh -J lxplus.cern.ch,cmsusr.cern.ch cmsdropbox.cms

# scp jumphost example
## ssh -J your.jump.host remote.internal.host
# scp -o 'ProxyJump your.jump.host' myfile.txt remote.internal.host:/my/dir
