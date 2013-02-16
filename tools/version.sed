#
# version.sed - extract version name from doc/changelog
#
# Copyright (c) 2003 EmuTOS development team.
#
# Authors:
#  LVL     Laurent Vogel
#
# This file is distributed under the GPL, version 2 or at your
# option any later version.  See doc/license.txt for details.
#

1i\
/* generated from doc/changelog.txt */

/^\* *Release: /{
  s///
  s/[ *]*$//
  :good
  # turn to C syntax
  s/["\]/\\&/g
  s/.*/const char version[] = "&";/
  # print and exit if on the last line
  ${
    p
    d
  }
  # else hold
  h
  d
}
/^[0-9]/{
  s/ *$//
  s/.*/(CVS &)/
  b good
}
#if on the last line, print the last saved good line
${
  g
  q
}

# any other line is junk
d
