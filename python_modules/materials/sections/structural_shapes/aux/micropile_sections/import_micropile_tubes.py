# -*- coding: utf-8 -*-
''' Import common micropile tubes data from:

https://braxima.com/tubos-micropilotes/

With some elaboration (see spreadsheet formulas).
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from misc_utils import spreadsheet_utils as su
import json

fNameIn= "common_micropile_tubes.xlsx"
fNameOut= '../common_micropile_tubes.json'
fNameOut= './pp.json'

columnOrder= ['nmb', 'D', 'e', 'P', 'A', 'slendernessRatio', 'Iz', 'iz', 'Wzel', 'Wzpl', 'It', 'Wt', 'AL', 'AG', '']

columnUnits= {'D':1e-3, 'e':1e-3, 'A':1e-4, 'Iz':1e-8, 'iz':1e-2, 'Wzel':1e-6, 'Wzpl':1e-6, 'Iy':1e-8, 'iy':1e-2, 'Wyel':1e-6, 'Wypl':1e-6, 'It':1e-8,  'Wt':1e-6}

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
        shapeRecord['Iy']= shapeRecord['Iz']
        shapeRecord['iy']= shapeRecord['iz']
        shapeRecord['Wyel']= shapeRecord['Wzel']
        shapeRecord['Wypl']= shapeRecord['Wzpl']
        retval[shapeName]= shapeRecord
    return retval

namePrefix= 'MP'

shapesDict= dict()
shapesDict[namePrefix]= su.read_shapes_from_spreadsheet(namePrefix= namePrefix, columnOrder= columnOrder, columnUnits= columnUnits, spreadsheetFileName= fNameIn, E= 210000e6, nu= 0.3, post_processing_function= post_process)

jsonFile= open(fNameOut, "w")
jsonFile.write(json.dumps(shapesDict))
jsonFile.close()
