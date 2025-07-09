# -*- coding: utf-8 -*-
''' Import rail profiles from British Steel Steel products for rail applications.

https://britishsteel.co.uk/media/bbnmwpai/british-steel-rail-profiles-and-grades-datasheet.pdf

'''


__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from misc_utils import spreadsheet_utils as su
import json

# Data obtained from: https://britishsteel.co.uk/media/bbnmwpai/british-steel-rail-profiles-and-grades-datasheet.pdf

# Section axis: 

#    British              XC
#  Steel catalog

#       ^ Y           ^ Y                    
#       |             |

#     -----         -----
#       |             | 
#       | -> X        | -> Z
#       |             |
#     -----         -----


# The axis used in the British Steel catallog are different from those used
# in XC (strong axis parallel to z axis) in other words: values for X
# and Z axis are swapped with respect to those in the catalog.

fNameIn= "en_13674-1_rail_sections.xlsx"
fNameOut= '../bs_en_13674-1_rail_sections.json'

columnOrder= ['nmb', 'equivalentName', 'P', 'h', 'head_width', 't', 'foot_width', 'Ix', 'Iy']
numColumns= len(columnOrder)


columnUnits= {'h':1e-3, 'head_width':1e-3, 't':1e-3, 'foot_width':1e-4, 'Ix':1e-8, 'Iy':1e-8}

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
        shapeRecord['Iz']= shapeRecord['Ix']
        shapeRecord['A']= shapeRecord['P']/7850.0
        retval[shapeName]= shapeRecord
    return retval

namePrefix= 'RAIL_'

shapesDict= su.read_shapes_from_spreadsheet(namePrefix= namePrefix, columnOrder= columnOrder, columnUnits= columnUnits, spreadsheetFileName= fNameIn, E= 210000e6, nu= 0.3, post_processing_function= post_process)

jsonFile= open(fNameOut, "w")
jsonFile.write(json.dumps(shapesDict))
jsonFile.close()
