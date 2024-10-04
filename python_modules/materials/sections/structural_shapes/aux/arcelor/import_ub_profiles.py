# -*- coding: utf-8 -*-
''' Import universal column data from arcelor orange book spreadsheet.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from misc_utils import spreadsheet_utils as su
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

fNameIn= "arcelor_ub_profiles.xlsx"
fNameOut= '../arcelor_ub_profiles.json'

columnOrder= ['nmb', '', 'P', 'h', 'b', 'tw', 'tf', 'r','d', 'cw_tw', 'cf_tf', 'C', 'N', 'n', 'AL', 'AG', 'Iz', 'Iy', 'iz', 'iy', 'Wzel', 'Wyel', 'Wzpl', 'Wypl', 'U', 'X', 'Iw', 'It', 'A']
columnUnits= {'h':1e-3, 'b':1e-3, 'tw':1e-3, 'tf':1e-3, 'r':1e-3,'d':1e-3, 'C':1e-3, 'N':1e-3, 'n':1e-3, 'Iz':1e-8, 'Iy':1e-8, 'iz':1e-2, 'iy':1e-2, 'Wzel':1e-6, 'Wyel':1e-6, 'Wzpl':1e-6, 'Wypl':1e-6, 'Iw':1e-6, 'It':1e-8, 'A':1e-4}


def post_process(shapesDict):
    ''' Post-process data readed from the spreadsheet

    :param shapesDict: dictionary containing the data imported from the
                       spreadsheet.
    '''
    # Some post-processing:
    for shapeName in shapesDict:
        shapeRecord= shapesDict[shapeName]
        for key in shapeRecord:
            if(key=='nmb'):
                value= shapeRecord[key].replace(' ','')
                shapeRecord[key]= value
    return shapesDict
                
shapesDict= su.read_shapes_from_spreadsheet(namePrefix= 'UB', columnOrder= columnOrder, columnUnits= columnUnits, spreadsheetFileName= fNameIn, E= 210000e6, nu= 0.3, post_processing_function= post_process)
                
jsonFile= open(fNameOut, "w")
jsonFile.write(json.dumps(shapesDict))
jsonFile.close()
