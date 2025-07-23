# -*- coding: utf-8 -*-
''' Import round bar shapes from Arcelor steel products.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from misc_utils import spreadsheet_utils as su
import json

fNameIn= "barras_redondas_acero_laminado_arcelor.xlsx"
fNameOut= '../arcelor_r_profiles.json'

columnOrder= ['nmb', 'P', 'd', 'A']
numColumns= len(columnOrder)
columnUnits= {'A':1e-4, 'd':1e-3}

def estimate_st_venant_torsion_constant(shape):
    ''' Return an the St. Venant torsion constant for circular solid sections.
        For a circular cross-section, the St. Venant torsion constant (J) is 
        equivalent to the polar moment of inertia, calculated as 
        J = (π * d^4)/32, where 'd' is the diameter of the circular section. 
        This constant quantifies a circular section's resistance to twisting 
        or torsional deformation. 

    :param shape: shape to compute the warping constant for.
    '''
    d= shape['d']
    return math.pi*d**4/32.0

def estimate_av(shape):
    ''' Return a the portion of the section that is subjected to shear forces.

    :param shape: shape to compute the warping constant for.
    '''
    A= shape['A']
    return 3/4.0*A

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
        d= shapeRecord['d']
        I= math.pi/64.0*d**4
        shapeRecord['Iz']= I
        shapeRecord['iz']= d/4.0
        Wel= I/(d/2.0)
        Wpl= 4/3.0*(d/2)**3
        shapeRecord['Wzel']= Wel
        shapeRecord['Wzpl']= Wpl
        shapeRecord['Iy']= I
        shapeRecord['iy']= d/4.0
        shapeRecord['Wyel']= Wel
        shapeRecord['Wypl']= Wpl
        av= estimate_av(shapeRecord)
        shapeRecord['Avy']= av
        shapeRecord['Avz']= av
        shapeRecord['Cw']= 0.0 # no warping under torsion.
        shapeRecord['It']= estimate_st_venant_torsion_constant(shapeRecord)
        retval[shapeName]= shapeRecord
    return retval

namePrefix= 'R'

shapesDict= su.read_shapes_from_spreadsheet(namePrefix= namePrefix, columnOrder= columnOrder, columnUnits= columnUnits, spreadsheetFileName= fNameIn, E= 210000e6, nu= 0.3, post_processing_function= post_process)

jsonFile= open(fNameOut, "w")
jsonFile.write(json.dumps(shapesDict))
jsonFile.close()
