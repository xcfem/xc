# -*- coding: utf-8 -*-
''' Import universal column data from SteelConstruction.info blue book spreadsheet.

https://www.steelforlifebluebook.co.uk/hfshs/ec3-ukna/section-properties-dimensions-properties/

'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

#from misc_utils import spreadsheet_utils as su
import spreadsheet_utils_link as su
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

fNameIn= "bs_en_10219.xlsx"
fNameOut= '../bs_en_10219_profiles.json'

columnOrder= dict()
columnOrder['CFSHS']= ['','e','b','nmb', 'P', 'A', 'Iz', 'iz', 'Wzel', 'Wzpl', 'It', 'Wt', 'AL', '', 'AG', '', 'hSlendernessRatio']
columnOrder['RHS']= ['','e', 'b', 'h', 'nmb', 'P', 'A', 'Iz', 'Iy', 'iz', 'iy', 'Wzel', 'Wyel', 'Wzpl', 'Wypl', 'It', 'Wt', 'AL', '', 'AG', 'bSlendernessRatio', 'hSlendernessRatio']
columnOrder['CFCHS']= ['D','e','nmb', 'P', 'A', 'Iz', 'iz', 'Wzel', 'Wzpl', 'It', 'Wt', 'AL', 'AG', '', 'slendernessRatio']

columnUnits= {'b':1e-3, 'h':1e-3, 'e':1e-3, 'A':1e-4, 'Iz':1e-8, 'iz':1e-2, 'Wzel':1e-6, 'Wzpl':1e-6, 'Iy':1e-8, 'iy':1e-2, 'Wyel':1e-6, 'Wypl':1e-6, 'It':1e-8,  'Wt':1e-6}

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

def post_process(shapesDict):
    ''' Post-process data readed from the spreadsheet

    :param shapesDict: dictionary containing the data imported from the
                       spreadsheet.
    '''
    # Some post-processing:
    retval= dict()
    for shapeName in shapesDict:
        shapeRecord= dict()
        shapeRecord.update(shapesDict[shapeName])
        if('CheckAvailability' in shapeRecord):
            shapeRecord['CheckAvailability']= (shapeRecord['CheckAvailability']=='#')
        if('h' in shapeRecord): # Square or rectangular shape
            outsideCornerRadius= getOutsideCornerRadii(shapeRecord['e'])
            insideCornerRadius= outsideCornerRadius-shapeRecord['e']
            shapeRecord['outsideCornerRadius']= outsideCornerRadius
            shapeRecord['insideCornerRadius']= insideCornerRadius
            h_flat= shapeRecord['h']-outsideCornerRadius
            shapeRecord['h_flat']= h_flat
            b_flat= shapeRecord['b']-outsideCornerRadius
            shapeRecord['b_flat']= b_flat
        if(shapeName.startswith('CFSHS') or shapeName.startswith('CFCHS')): # Square or circular section
            shapeRecord['Iy']= shapeRecord['Iz']
            shapeRecord['iy']= shapeRecord['iz']
            shapeRecord['Wyel']= shapeRecord['Wzel']
            shapeRecord['Wypl']= shapeRecord['Wzpl']
        if(shapeName.startswith('CFSHS')): # Square section
           # A= shapeRecord['A']
           E= shapeRecord['E']
           nu= shapeRecord['nu']
           b= shapeRecord['b']
           shapeRecord['h']= shapeRecord['b']
           h= b
           shapeRecord['bSlendernessRatio']=  shapeRecord['hSlendernessRatio']
           e= shapeRecord['e']
           shapeRecord['alpha']= 0.5*5/6.0
           shapeRecord['G']= E/(2*(1+nu))
           shapeRecord['Avy']= 2*0.7*h*e
           shapeRecord['Avz']= 2*0.7*b*e
           shapeRecord['Wyel']= shapeRecord['Wzel']
           shapeRecord['Wypl']= shapeRecord['Wzpl']
        retval[shapeName]= shapeRecord
    return retval

shapesDict= dict()
for namePrefix in ['CFCHS', 'CFSHS', 'RHS']:
    shapesDict[namePrefix]= su.read_shapes_from_spreadsheet(namePrefix= namePrefix, columnOrder= columnOrder[namePrefix], columnUnits= columnUnits, spreadsheetFileName= fNameIn, E= 210000e6, nu= 0.3, post_processing_function= post_process)
    
jsonFile= open(fNameOut, "w")
jsonFile.write(json.dumps(shapesDict))
jsonFile.close()
