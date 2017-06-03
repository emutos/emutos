#!/bin/bash -eu
# -e: Exit immediately if a command exits with a non-zero status.
# -u: Treat unset variables as an error when substituting.

if [ $UID != 0 ]
then
  echo "error: This script needs to be run with root access rights." 2>&1
  exit 1
fi

# In order to avoid security warnings when installing packages from third-party
# APT repositories, we need to trust their public key. Instead of downloading
# the public keys every time, we download them once and we store them here.
# So if the remote website gets hijacked and provides packages signed with a
# different key, we will not inadvertently install bad packages.

# Display expanded script commands
set -x

# Vincent Rivière's m68k-atari-mint cross-tools
# http://vincent.riviere.free.fr/soft/m68k-atari-mint/ubuntu.php
apt-key add .travis/vr-apt-key.txt
echo "deb http://vincent.riviere.free.fr/debian/ $(lsb_release -cs) contrib" >>/etc/apt/sources.list

# Update the packages list
apt-get update -qq
