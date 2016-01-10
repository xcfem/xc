#!/bin/bash
for fl in $(find -type f -name '*.py' )
do
echo $fl
sed -i 's/hormigonesEHE/EHE_concrete/g' $fl
# sed -i 's/RecordSeccionHALosa/RecordRCSlabSection/g' $fl
done
