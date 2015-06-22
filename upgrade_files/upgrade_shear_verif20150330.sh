#!/bin/sh
echo "*** 24/03/2015 Revision of shear calculations according to SIA262."
echo "procesando: " $1
cp -i $1 $1.bak
cat $1.bak  | sed 's/setArmInfT/setMainReinf2pos/g' \
| sed 's/setArmSupT/setMainReinf2neg/g' \
| sed 's/setArmInfL/setMainReinf1pos/g' \
| sed 's/setArmSupL/setMainReinf1neg/g' \
> $1
