#!/bin/sh

fromdos $1
cat $1 | grep "point, X=" > /tmp/$1.txt

cat /tmp/$1.txt | sed 's/en point, X\=/Base.Vector\(/' \
| sed 's/Y\=/,/g'| sed 's/Z\=/,/g' | sed 's/[ \t]*//g'| sed 's/$/),/g' >/tmp/$1.1.txt

echo "# -*- coding: utf-8 -*-" > $2
echo "import sys" >> $2
echo "sys.path.append(\"/usr/lib/freecad/lib\")" >> $2

echo "import FreeCAD" >> $2
echo "from FreeCAD import Base" >> $2
echo "from FreeCAD import Part" >> $2
echo "points= [" >> $2
cat /tmp/$1.1.txt >> $2
echo "]" >> $2

