#
# converts a 32x32 icon definition into 'bitmap-text' format
# use with caution!
# usage: sed -f icn2txt fooh.icn >foo.txt
#

/,/!{
  p
  d
}

s/{ 0x//
s/  0x//
s/, 0x//
s/, 0x/\
/
s/, 0x//
s/, //
s/0/..../g
s/1/...X/g
s/2/..X./g
s/3/..XX/g
s/4/.X../g
s/5/.X.X/g
s/6/.XX./g
s/7/.XXX/g
s/8/X.../g
s/9/X..X/g
s/A/X.X./g
s/B/X.XX/g
s/C/XX../g
s/D/XX.X/g
s/E/XXX./g
s/F/XXXX/g


