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

fNameIn= "bs_en_10219.ods"
fNameOut= '../bs_en_10219_profiles.json'

columnOrder= dict()
columnOrder['CFSHS']= ['','e','b','nmb', 'P', 'A', 'Iz', 'iz', 'Wzel', 'Wzpl', 'It', 'Wt', 'AL', '', 'AG', '', 'hSlendernessRatio']
columnOrder['RHS']= ['','e', 'b', 'h', 'nmb', 'P', 'A', 'Iz', 'Iy', 'iz', 'iy', 'Wzel', 'Wyel', 'Wzpl', 'Wypl', 'It', 'Wt', 'AL', '', 'AG', 'bSlendernessRatio', 'hSlendernessRatio']
columnOrder['CFCHS']= ['D','e','nmb', 'P', 'A', 'Iz', 'iz', 'Wzel', 'Wzpl', 'It', 'Wt', 'AL', 'AG', '', 'slendernessRatio']


columnUnits= {'b':'e-3', 'h':'e-3', 'e':'e-3', 'P':'', 'A':'e-4', 'hSlendernessRatio':'', 'bSlendernessRatio':'', 'slendernessRatio':'', 'Iz':'e-8', 'iz':'e-2', 'Wzel':'e-6', 'Wzpl':'e-6', 'Iy':'e-8', 'iy':'e-2', 'Wyel':'e-6', 'Wypl':'e-6', 'It':'e-8',  'Wt':'e-6'}

def getOutsideCornerRadii(thickness):
    ''' Return the radius of the outside corners.'''
    retval= thickness
    if(thickness<= 6e-3):
        retval*= 2.0
    elif(thickness<=10e-3):
        retval*=2.5
    else:
        retval*= 3.0
    return retval

def importSections(rows, namePrefix):
    ''' Import the data corresponding to cold formed
        square hollow sections.'''
    cOrder= columnOrder[namePrefix]
    numColumns= len(cOrder)
    columnKeys= dict()
    for count, key in enumerate(cOrder):
        if(len(key)>0):
            columnKeys[key]= count
    for r in rows:
        if(len(r)>=numColumns): # if not empty.
            name= r[columnKeys['nmb']].replace(' ','')
            if(name.startswith(namePrefix)):
                shapeRecord= dict()
                for key in cOrder:
                    if(len(key)>0):
                        column= columnKeys[key]
                        shapeRecord[key]= r[column]
                if(namePrefix=='CFSHS'): # Square section
                    shapeRecord['h']= shapeRecord['b']
                    shapeRecord['bSlendernessRatio']=  shapeRecord['hSlendernessRatio']
                if(namePrefix in ['CFSHS','CFCHS']): # Square or circular section
                    shapeRecord['Iy']= shapeRecord['Iz']
                    shapeRecord['iy']= shapeRecord['iz']
                    shapeRecord['Wyel']= shapeRecord['Wzel']
                    shapeRecord['Wypl']= shapeRecord['Wzpl']
                    
                shapeRecord['E']= 210000e6 # To deprecate
                shapeRecord['nu']= 0.3 # To deprecate
                shapesDict[name]= shapeRecord
    

    
shapesDict= dict() # Dictionary with all the shapes.
data= get_data(fNameIn)
for sheetName in data:
    rows= data[sheetName]
    #if(sheetName in ['CFSHS', 'RHS']):
    importSections(rows= rows, namePrefix= sheetName)
    
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
            value= str(shapeRecord[key])#.replace(',','')
            shapeRecord[key]= float(value+factor)
    if('h' in shapeRecord): # Square or rectangular shape
        outsideCornerRadius= getOutsideCornerRadii(shapeRecord['e'])
        insideCornerRadius= outsideCornerRadius-shapeRecord['e']
        shapeRecord['outsideCornerRadius']= outsideCornerRadius
        shapeRecord['insideCornerRadius']= insideCornerRadius
        h_flat= shapeRecord['h']-outsideCornerRadius
        shapeRecord['h_flat']= h_flat
        b_flat= shapeRecord['b']-outsideCornerRadius
        shapeRecord['b_flat']= b_flat
            
jsonFile= open(fNameOut, "w")
jsonFile.write(json.dumps(shapesDict))
jsonFile.close()

