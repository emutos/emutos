#
# mkdiff : a quick wrapper around diff 
#

# define AUTHOR to be your initials, XYZ.
# will create a diff between a pristine emutos directory
# and a directory called xyzemutos containing the changes 
# of author XYZ.
#
AUTHOR=MAD
PROJECT=emutos

#
# do not alter below.
#
LCAUTHOR=`echo $AUTHOR | tr A-Z a-z`

DATE=`date +%y%m%d`
FILE="${PROJECT}_${DATE}_${LCAUTHOR}.diff"
cat <<EOT >$FILE
date `date`
the patch was created using the command:

  LC_ALL=C T2=UTC0 diff -Naur ${PROJECT} ${LCAUTHOR}${PROJECT} >>${FILE}

to apply the patch, go to the directory containing ${PROJECT} (NOT the
${PROJECT} directory), and do

  patch -p0 <${FILE}

(signed) ${AUTHOR}
-------------------------------------------------------------------------
EOT

LC_ALL=C T2=UTC0 diff -Naur ${PROJECT} ${LCAUTHOR}${PROJECT} >>${FILE}
gzip ${FILE}
echo "gzipped patch ${FILE}.gz done."
