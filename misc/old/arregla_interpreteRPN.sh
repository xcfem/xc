#!/bin/sh
echo "procesando: " $1
cp -i $1 $1.bak
cat $1.bak  | sed 's/interpreteRPN.string_to/interpreta/g' \
> $1