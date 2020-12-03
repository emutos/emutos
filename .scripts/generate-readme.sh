#!/bin/bash -eu
# -e: Exit immediately if a command exits with a non-zero status.
# -u: Treat unset variables as an error when substituting.

# Generates a Markdown README with informations about the build

# In the YAML file...
# COMPARE_URL needs to be set to ${{ github.event.compare }},
# PREVIOUS_COMMIT needs to be set to ${{ github.event.before }},
# CURRENT_COMMIT needs to be set to ${{ github.event.after }},

if ! git merge-base --is-ancestor $PREVIOUS_COMMIT $CURRENT_COMMIT 2>/dev/null
then
  # Forced push: use parent commit as previous commit
  PREVIOUS_COMMIT=$CURRENT_COMMIT~
fi

# Normalize commits
PREVIOUS_COMMIT=$(git rev-parse --short $PREVIOUS_COMMIT)
CURRENT_COMMIT=$(git rev-parse --short $CURRENT_COMMIT)
COMMIT_RANGE=$PREVIOUS_COMMIT..$CURRENT_COMMIT

echo "These binaries have been produced by"
echo "[GitHub Actions build #$GITHUB_RUN_ID]($GITHUB_SERVER_URL/$GITHUB_REPOSITORY/actions/runs/$GITHUB_RUN_ID)"
echo "for commits [$COMMIT_RANGE]($COMPARE_URL)."
echo

# Generate log as preformatted text with hyperlinks
git log --name-status $COMMIT_RANGE -- | sed \
  -e 's|.*|``&``  |' \
  -e 's|[a-z]\+://[^ `]*|``\[&\](&)``|g' \
  -e 's|commit \([0-9a-f]\+\)|commit`` \[\1\]('$GITHUB_SERVER_URL/$GITHUB_REPOSITORY'/commit/\1) ``|g' \
  -e 's|````||g'
