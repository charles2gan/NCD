#!/bin/bash

NAME1=$(basename $1)
rm /tmp/$NAME1.7z
cat $1 | 7za a -mx9 -ms=4g -md=100m -t7z -si /tmp/$NAME1.7z

#High precision
CX=$( wc -c /tmp/$NAME1.7z | sed 's/^[ \t]*//' | cut -f1 -d' ')
X=$( wc -c $1 | sed 's/^[ \t]*//' | cut -f1 -d' ')
CR=$(echo "$CX/$X" | bc -l)
echo $CR
