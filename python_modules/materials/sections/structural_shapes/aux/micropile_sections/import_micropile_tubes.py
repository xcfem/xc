# -*- coding: utf-8 -*-
''' Import common micropile tubes data from:

https://braxima.com/tubos-micropilotes/

With some elaboration (see spreadsheet formulas).
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from pyexcel_ods import get_data
import json

fNameIn= "common_micropile_tubes.ods"
fNameOut= '../common_micropile_tubes.json'

columnOrder= dict()
columnOrder['MP']= ['nmb', 'D', 'e', 'P', 'A', 'slendernessRatio', 'Iz', 'iz', 'Wzel', 'Wzpl', 'It', 'Wt', 'AL', 'AG', '']

columnUnits= {'e':'e-3', 'P':'', 'A':'e-4', 'slendernessRatio':'', 'Iz':'e-8', 'iz':'e-2', 'Wzel':'e-6', 'Wzpl':'e-6', 'Iy':'e-8', 'iy':'e-2', 'Wyel':'e-6', 'Wypl':'e-6', 'It':'e-8',  'Wt':'e-6'}

def importSections(rows, namePrefix):
    ''' Import the data corresponding to cold formed
        square hollow sections.'''
    retval= dict()
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
                shapeRecord['Iy']= shapeRecord['Iz']
                shapeRecord['iy']= shapeRecord['iz']
                shapeRecord['Wyel']= shapeRecord['Wzel']
                shapeRecord['Wypl']= shapeRecord['Wzpl']
                    
                shapeRecord['E']= 210000e6 # To deprecate
                shapeRecord['nu']= 0.3 # To deprecate
                retval[name]= shapeRecord
    return retval

shapesDict= dict() # Dictionary with all the shapes.
data= get_data(fNameIn)
for sheetName in data:
    rows= data[sheetName]
    #if(sheetName in ['CFSHS', 'RHS']):
    shapesDict[sheetName]= importSections(rows= rows, namePrefix= 'MP')
    
# Some post-processing:
for shapePrefix in shapesDict:
    sheetDict= shapesDict[shapePrefix]
    for shapeName in sheetDict:
        shapeRecord= sheetDict[shapeName]
        for key in shapeRecord:
            if key in columnUnits:
                factor= columnUnits[key]
                value= str(shapeRecord[key])#.replace(',','')
                shapeRecord[key]= float(value+factor)
    
jsonFile= open(fNameOut, "w")
jsonFile.write(json.dumps(shapesDict))
jsonFile.close()
