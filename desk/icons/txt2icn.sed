#
# converts back a 32x32 icon definition from text to icn format
# use with even more caution!
# usage: sed -f txt2icn foo.txt >fooh.icn
# note: the resulting .icn file is exactly the same as the
# original, except for the last comma which I didn't bother to 
# care for.
#

1{
  x
  s/$/{ /
  x
}

/[^.X]/{
  p
  d
}

s/\(....\)/_\1/g
s/_\.\.\.\./0/g
s/_\.\.\.X/1/g
s/_\.\.X\./2/g
s/_\.\.XX/3/g
s/_\.X\.\./4/g
s/_\.X\.X/5/g
s/_\.XX\./6/g
s/_\.XXX/7/g
s/_X\.\.\./8/g
s/_X\.\.X/9/g
s/_X\.X\./A/g
s/_X\.XX/B/g
s/_XX\.\./C/g
s/_XX\.X/D/g
s/_XXX\./E/g
s/_XXXX/F/g

s/\(....\)\(....\)/0x\1, 0x\2, /
H
g
/\n.*\n/{
  s/\n//g
  p
  s/.*/  /
  h
}
d

