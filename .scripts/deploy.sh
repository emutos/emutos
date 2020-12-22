#!/bin/bash -eu
# -e: Exit immediately if a command exits with a non-zero status.
# -u: Treat unset variables as an error when substituting.

# This script deploys the built binaries to SourceForge:
# https://sourceforge.net/projects/emutos/files/

# To enable passwordless SSH:
# - Our private/public key pair must have been installed (see install_ssh_id.sh)
# - Our public key must have been authorized on SourceForge, in the account of
#   a user which has write access to the project's files:
#   https://sourceforge.net/auth/shell_services

if [ -z ${VERSION+x} ]
then
  echo "error: VERSION is undefined" >&2
  exit 1
fi

LOCAL_DIRNAME=release-archives
REMOTE_DIRNAME=$VERSION

# SourceForge variables
SF_PROJECT=emutos
SF_USER=vriviere
SF_DIR=snapshots

# SSH variables for SourceForge
SSH_USER=$SF_USER,$SF_PROJECT
SSH_HOST=frs.sourceforge.net
SSH_PATH=/home/pfs/p/$SF_PROJECT/$SF_DIR

echo "Deploying $LOCAL_DIRNAME to $SSH_HOST:$SSH_PATH/$REMOTE_DIRNAME"
echo "See result at https://sourceforge.net/projects/$SF_PROJECT/files/$SF_DIR/$REMOTE_DIRNAME/"
echo '$ lftp'
cat << EOF | tee /dev/stderr | lftp
set sftp:connect-program "ssh -a -x -o StrictHostKeyChecking=no"
open sftp://$SSH_USER:@$SSH_HOST$SSH_PATH
mirror -R $LOCAL_DIRNAME $REMOTE_DIRNAME
ls | .travis/generate-purge.sh >purge.lftp
source purge.lftp
EOF
