#!/bin/sh
sed 's/@tan/math.tan/g' $1 | \
sed 's/@exp/math.exp/g' | \
sed 's/@sqrt/math.sqrt/g' | \
sed 's/@pow/math.pow/g' | \
sed 's/@PI/math.pi/g' | \
sed 's/@sin/math.sin/g' | \
sed 's/@cos/math.cos/g' | \
sed 's/@atan2/math.atan2/g' | \
sed 's/\\def\_prop\["/def /g' | \
sed 's/\^/**/g' | \
sed 's/","func"\]//g' | \
sed 's/string //g' | \
sed 's/double //g' | \
sed 's/\\tipo{"double"}//g' | \
sed 's/\\return{/return /g' | \
sed 's/\\c{//g' | \
sed 's/\\def_var\["//g' | \
sed 's/","double"]{/=/g' | \
sed 's/{//g' | \
sed 's/}//g' | \
sed 's/@//g' 
