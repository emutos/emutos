#! /bin/sh

#
# test compr/uncompr on various kind of input and checking the the output
# of uncompr is identical to the input of compr.
#

TMP=.compr

rm -rf $TMP
mkdir $TMP
cd $TMP

cat >t <<EOF
u="\$*"
echo testing \$u
rm -f b c
../compr a b
if test -f b 
then 
  ../uncompr b c
  if test -f c
  then
    if diff a c 
    then
      echo \$u: success
    else 
      echo \$u: comparison failed
    fi
  else
    echo \$u: uncompression failed
  fi
else
  echo \$u: compression failed
fi
rm -f a b c
EOF


cp ../readme.txt a
. t readme.txt

if test -f ../ramtos.img 
then
  cp ../ramtos.img a
  . t ramtos.img
fi



echo -n aaaaaaaaaaaa > a
. t "aaaa"

awk '
BEGIN {
  for(i = 32; i < 127; i++) {
    for(j = 32; j < 127; j++) {
      for(k = 32; k < 50; k++) {
        printf("%c%c%c\n", i, j, k)
      }
    }
  }
}' >a
. t "non-compressible data"

>a
. t "empty file"

# cleanup
cd ..
rm -rf $TMP


