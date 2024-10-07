# -*- coding: utf-8 -*-
''' Import universal column data from SteelConstruction.info blue book spreadsheet.

https://www.steelforlifebluebook.co.uk/hfshs/ec3-ukna/section-properties-dimensions-properties/

'''


__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from misc_utils import spreadsheet_utils as su
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

fNameIn= "HFSHS-secpropsdimsprops-EC3-UKNA-UK-18_01_2022.xlsx"
fNameOut= '../bs_en_10210_hfshs_profiles.json'

columnOrder= ['','','','nmb', 'b', 'e', 'CheckAvailability', 'P', 'A', 'hSlendernessRatio', 'Iz', 'iz', 'Wzel', 'Wzpl', 'It', 'Wt', 'AL', 'AG']
numColumns= len(columnOrder)


columnUnits= {'b':1e-3,'h':1e-3, 'e':1e-3, 'A':1e-4, 'Iz':1e-8, 'iz':1e-2, 'Wzel':1e-6, 'Wzpl':1e-6, 'Iy':1e-8, 'iy':1e-2, 'Wyel':1e-6, 'Wypl':1e-6, 'It':1e-8,  'Wt':1e-6}

def post_process(shapesDict):
    ''' Post-process data readed from the spreadsheet

    :param shapesDict: dictionary containing the data imported from the
                       spreadsheet.
    '''
    # Some post-processing:
    retval= dict()
    for shapeName in shapesDict:
        retval[shapeName]= shapesDict[shapeName]
        shapeRecord= dict()
        shapeRecord.update(retval[shapeName])
        shapeRecord['CheckAvailability']= (shapeRecord['CheckAvailability']=='#')
        shapeRecord['h']= shapeRecord['b']
        shapeRecord['Iy']= shapeRecord['Iz']
        shapeRecord['iy']= shapeRecord['iz']
        shapeRecord['Wyel']= shapeRecord['Wzel']
        shapeRecord['Wypl']= shapeRecord['Wzpl']
        shapeRecord['bSlendernessRatio']=  shapeRecord['hSlendernessRatio']
        h_flat= shapeRecord['h']-3.0*shapeRecord['e']
        shapeRecord['h_flat']= h_flat
        b_flat= shapeRecord['b']-3.0*shapeRecord['e']
        shapeRecord['b_flat']= b_flat
        retval[shapeName]= shapeRecord
    return retval

namePrefix= 'HFSHS'

shapesDict= su.read_shapes_from_spreadsheet(namePrefix= namePrefix, columnOrder= columnOrder, columnUnits= columnUnits, spreadsheetFileName= fNameIn, E= 210000e6, nu= 0.3, post_processing_function= post_process)

jsonFile= open(fNameOut, "w")
jsonFile.write(json.dumps(shapesDict))
jsonFile.close()
