#!/bin/bash -eu
# -e: Exit immediately if a command exits with a non-zero status.
# -u: Treat unset variables as an error when substituting.

if [ $UID != 0 ]
then
  echo "error: This script needs to be run with root access rights." 2>&1
  exit 1
fi

# Display expanded script commands
set -x

# Vincent Rivière's m68k-atari-mint cross-tools
# http://vincent.riviere.free.fr/soft/m68k-atari-mint/ubuntu.php
sudo add-apt-repository -y ppa:vriviere/ppa

# Update the packages list
apt-get update -qq
