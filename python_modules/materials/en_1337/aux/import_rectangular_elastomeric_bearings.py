# -*- coding: utf-8 -*-
''' EN 1337-3:2005 (E) Table 3 — Standard sizes for bearings type B.
    Rectangular elastomeric bearings.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import csv
import json

fNameIn= 'table_3_en_1337-3_2005_rectangular_elastomeric_bearings.csv'
fNameOut= './'+fNameIn.replace('.csv', '.json')

# diam: bearing diameter.
# a: bearing length (see figure 2 of EN 1337-3:2005).
# b: bearing width (see figure 2 of EN 1337-3:2005).
# tb: total height (see figure 2 of EN 1337-3:2005).
# ti: thickness of individual elastomer layer (see figure 2 of EN 1337-3:2005).
# ts: thickness of steel reinforcing plate (see figure 2 of EN 1337-3:2005).
# tso: thickness of outer steel reinforcing plate.
# Te: total elastomer thickness.
# Tb: total nominal thickness of bearing.
# n: number of elastomer layers.
# C: side cover.
# ted: edge cover.

with open(fNameIn) as csv_file:
    data_dict= dict()
    rows = csv.reader(csv_file, delimiter=';')
    line_count = 0
    for row in rows:
        dimensions= row[0].strip()
        if(len(dimensions)>0):
            if(dimensions[0:3].isdigit()):
                # Read dimensionseter.
                str_dimensions= dimensions.strip().replace(' ','')
                dimensions= str_dimensions.split('x')
                # Read bearing length
                a= float(dimensions[0])*1e-3
                # Read bearing width
                b= float(dimensions[1])*1e-3
                # Read elastomer layer thickness.
                ti= float(row[5])*1e-3
                # Read reinforcing plate thickness.
                ts= float(row[6])*1e-3
                # Read minimum number of layers
                min_layer_number= int(row[7])
                # Read maxnimum number of layers
                max_layer_number= int(row[8])
                for ln in range(min_layer_number, max_layer_number+1):
                    total_elastomer_thickness= ln*ti
                    key= str_dimensions+'_'+str(int(total_elastomer_thickness*1e3))
                    total_steel_thickness= (ln+1)*ts
                    data_dict[key]= {'a':a, 'b':b, 'ti':ti, 'ts':ts,'number_of_layers':ln}
        line_count += 1
    print(f'Processed {line_count} lines.')
    
jsonFile= open(fNameOut, "w")
jsonFile.write(json.dumps(data_dict))
jsonFile.close()
