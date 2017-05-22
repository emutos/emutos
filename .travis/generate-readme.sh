#!/bin/bash -eu
# -e: Exit immediately if a command exits with a non-zero status.
# -u: Treat unset variables as an error when substituting.

echo "These binaries have been produced by"
echo "[Travis CI Build #$TRAVIS_BUILD_NUMBER](https://travis-ci.org/emutos/emutos/builds/$TRAVIS_BUILD_ID)"
echo "for commits [$TRAVIS_COMMIT_RANGE](https://github.com/emutos/emutos/compare/$TRAVIS_COMMIT_RANGE)."
echo

# The variable TRAVIS_COMMIT_RANGE includes 3 dots, which is suitable for
# "git diff". But as we are going to use "git log", we need 2 dots instead.
LOG_RANGE=$(echo $TRAVIS_COMMIT_RANGE | sed -n 's/\.\.\./../p')

# Generate log as preformatted text with hyperlinks
git log --name-status $LOG_RANGE -- | sed \
  -e 's|^commit \([0-9a-z]*\).*|``commit`` \[\1\](https://github.com/emutos/emutos/commit/\1)  |' \
  -e 's/^$/`` ``  /' \
  -e 's/^[^`].*/``&``  /'
