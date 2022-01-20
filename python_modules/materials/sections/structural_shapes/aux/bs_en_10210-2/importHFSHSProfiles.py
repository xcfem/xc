# -*- coding: utf-8 -*-
''' Import universal column data from SteelConstruction.info blue book spreadsheet.

https://www.steelforlifebluebook.co.uk/hfshs/ec3-ukna/section-properties-dimensions-properties/

'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from pyexcel_ods import get_data
import json

# Data obtained from: https://www.steelforlifebluebook.co.uk/hfshs/ec3-ukna/section-properties-dimensions-properties/

# Section axis: 

#    SCI              XC
#  Blue book

#       ^ Z           ^ Y                    
#       |             |

#     -----         -----
#       |             | 
#       | -> Y        | -> Z
#       |             |
#     -----         -----


# The axis used in the Blue Book documentation are different from those used
# in XC (strong axis parallel to z axis) in other words: values for Y
# and Z axis are swapped with respect to those in the catalog.

fNameIn= "HFSHS-secpropsdimsprops-EC3-UKNA-UK-18_01_2022.ods"
fNameOut= '../bs_en_10210_hfshs_profiles.json'

data= get_data(fNameIn)
columnOrder= ['','','','nmb', 'b', 'e', 'CheckAvailability', 'P', 'A', 'hSlendernessRatio', 'Iz', 'iz', 'Wzel', 'Wzpl', 'It', 'Wt', 'AL', 'AG']
numColumns= len(columnOrder)


columnUnits= {'b':'e-3','h':'e-3', 'e':'e-3', 'P':'', 'A':'e-4', 'hSlendernessRatio':'e-3', 'bSlendernessRatio':'e-3', 'Iz':'e-8', 'iz':'e-2', 'Wzel':'e-6', 'Wzpl':'e-6', 'Iy':'e-8', 'iy':'e-2', 'Wyel':'e-6', 'Wypl':'e-6', 'It':'e-8',  'Wt':'e-6'}

namePrefix= 'HFSHS'


columnKeys= dict()
for count, key in enumerate(columnOrder):
    if(len(key)>0):
        columnKeys[key]= count
    
shapesDict= dict() # Dictionary with all the shapes.
for sheet in data:
    rows= data[sheet]
    for r in rows:
        if(len(r)>=numColumns): # if not empty.
            name= r[columnKeys['nmb']].replace(' ','')
            if(name.startswith(namePrefix)):
                shapeRecord= dict()
                for key in columnOrder:
                    if(len(key)>0):
                        column= columnKeys[key]
                        shapeRecord[key]= r[column]
                shapeRecord['h']= shapeRecord['b']
                shapeRecord['Iy']= shapeRecord['Iz']
                shapeRecord['iy']= shapeRecord['iz']
                shapeRecord['Wyel']= shapeRecord['Wzel']
                shapeRecord['Wypl']= shapeRecord['Wzpl']
                shapeRecord['bSlendernessRatio']=  shapeRecord['hSlendernessRatio']
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
        if(key=='CheckAvailability'):
            value= shapeRecord[key]
            shapeRecord[key]= (value=='#')
                
        if key in columnUnits:
            factor= columnUnits[key]
            value= shapeRecord[key].replace(',','')
            shapeRecord[key]= float(value+factor)
    h_flat= shapeRecord['h']-3.0*shapeRecord['e']
    shapeRecord['h_flat']= h_flat
    b_flat= shapeRecord['b']-3.0*shapeRecord['e']
    shapeRecord['b_flat']= b_flat
            
jsonFile= open(fNameOut, "w")
jsonFile.write(json.dumps(shapesDict))
jsonFile.close()

