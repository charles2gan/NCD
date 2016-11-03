#!/bin/bash

NAME1=$(basename $1)
NAME2=$(basename $2)
rm /tmp/$NAME1.1.7z
rm /tmp/$NAME2.2.7z
rm /tmp/cat.$NAME1.$NAME2.7z
cat $1 | 7za a -mx9 -ms=4g -md=100m -t7z -si /tmp/$NAME1.1.7z
cat $2 | 7za a -mx9 -ms=4g -md=100m -t7z -si /tmp/$NAME2.2.7z
cat $1 $2 | 7za a -mx9 -ms=4g -md=100m -t7z -si /tmp/cat.$NAME1.$NAME2.7z

#High precision
CX=$( wc -c /tmp/$NAME1.1.7z | sed 's/^[ \t]*//' | cut -f1 -d' ')
CY=$( wc -c /tmp/$NAME2.2.7z | sed 's/^[ \t]*//' | cut -f1 -d' ')
CXY=$( wc -c /tmp/cat.$NAME1.$NAME2.7z | sed 's/^[ \t]*//' | cut -f1 -d' ')
MAXIMUM=$(($CX>$CY?$CX:$CY))
MINIMUM=$(($CX<$CY?$CX:$CY))
NCD=$(echo "($CXY-$MINIMUM)/$MAXIMUM" | bc -l)
echo $NCD
