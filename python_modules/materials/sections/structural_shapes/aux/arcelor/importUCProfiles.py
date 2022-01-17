# -*- coding: utf-8 -*-
''' Import universal column data from arcelor orange book spreadsheet.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from pyexcel_ods import get_data
import json

# Data obtained from: 

# Section axis: https://orangebook.arcelormittal.com/design-data/uk-na/columns/universal-columns/section-properties-dimensions-and-properties/

#    ARCELOR          XC
#  Orange book

#       ^ Z           ^ Y                    
#       |             |

#     -----         -----
#       |             | 
#       | -> Y        | -> Z
#       |             |
#     -----         -----


# The axis used in Arcelor documentation are different from those used in XC
# (strong axis parallel to z axis) in other words: values for Y and Z axis 
# are swapped with respect to those in the catalog.

fNameIn= "arcelor_uc_profiles.ods"
fNameOut= '../arcelor_uc_profiles.json'

data= get_data(fNameIn)

columnOrder= ['nmb', '', 'P', 'h', 'b', 'tw', 'tf', 'r','d', 'cw_tw', 'cf_tf', 'C', 'N', 'n', 'AL', 'AG', 'Iz', 'Iy', 'iz', 'iy', 'Wzel', 'Wyel', 'Wzpl', 'Wypl', 'U', 'X', 'Iw', 'It', 'A']
columnUnits= {'h':'e-3', 'b':'e-3', 'tw':'e-3', 'tf':'e-3', 'r':'e-3','d':'e-3', 'C':'e-3', 'N':'e-3', 'n':'e-3', 'Iz':'e-8', 'Iy':'e-8', 'iz':'e-2', 'iy':'e-2', 'Wzel':'e-6', 'Wyel':'e-6', 'Wzpl':'e-6', 'Wypl':'e-6', 'Iw':'e-6', 'It':'e-8', 'A':'e-4'}


columnKeys= dict()
for count, key in enumerate(columnOrder):
    if(len(key)>0):
        columnKeys[key]= count
    
shapesDict= dict() # Dictionary with all the shapes.
for sheet in data:
    rows= data[sheet]
    for r in rows:
        if(len(r)>0): # if not empty.
            name= r[columnKeys['nmb']].replace(' ','')
            if(name.startswith('UC')):
                shapeRecord= dict()
                for key in columnOrder:
                    if(len(key)>0):
                        column= columnKeys[key]
                        shapeRecord[key]= r[column]
                shapeRecord['E']= 210000e6 # To deprecate
                shapeRecord['nu']= 0.3 # To deprecate
                shapesDict[name]= shapeRecord

# Some post-processing:
for shapeName in shapesDict:
    shapeRecord= shapesDict[shapeName]
    for key in shapeRecord:
        if(key=='nmb'):
            value= shapeRecord[key].replace(' ','')
            shapeRecord[key]= value
        if key in columnUnits:
            factor= columnUnits[key]
            value= shapeRecord[key].replace(',','')
            shapeRecord[key]= float(value+factor)
            
jsonFile= open(fNameOut, "w")
jsonFile.write(json.dumps(shapesDict))
jsonFile.close()

