The obj/ directory contains the object files during compilation.
This readme.txt ensures that obj/ is not empty, so that doing 
 
  cvs update -d -P

or having the line

  update -d -P

in one's .cvrsc still creates the obj/ directory.
