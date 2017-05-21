#!/bin/bash -eu
# -e: Exit immediately if a command exits with a non-zero status.
# -u: Treat unset variables as an error when substituting.

echo "Built by Travis CI:"
echo "https://travis-ci.org/emutos/emutos/builds/$TRAVIS_BUILD_ID"
echo

# The variable TRAVIS_COMMIT_RANGE includes 3 dots, which is suitable for
# "git diff". But as we are going to use "git log", we need 2 dots instead.
LOG_RANGE=$(echo $TRAVIS_COMMIT_RANGE | sed -n 's/\.\.\./../p')
git log --name-status $LOG_RANGE --
