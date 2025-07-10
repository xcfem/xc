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

columnOrder= ['nmb', '', '', 'symbol', 'A', 'P', 'Yg', 'Ix', 'Iy', 'ix', 'iy', 'Wzel_head', 'Wzel_base', 'Wyel_head', 'Wyel_base', 'h', 'foot_width', 'head_width', 'tw', 'dimE', 'dimF', '', 'dimR']
numColumns= len(columnOrder)


columnUnits= {'A':1e-4, 'Yg':1e-3, 'Ix':1e-8, 'Iy':1e-8, 'ix':1e-6, 'iy':1e-6, 'Wzel_head':1e-6, 'Wzel_base':1e-6, 'Wyel_head':1e-6, 'Wyel_base':1e-6, 'h':1e-3, 'foot_width':1e-3, 'head_width':1e-3, 'tw':1e-3, 'dimE':1e-3, 'dimF':1e-3, 'dimR':1e-3}

def estimate_warping_constant(shape):
    ''' Return an estimation of the warping constant according to the 
        equation (14) of the article:

     Simplified Lateral Torsional Buckling Equations for Singly-Symmetric 
     I-Section Members of Donald W. White and Se-Kwon Jung.

    :param shape: shape to compute the warping constant for.
    '''
    D= shape['h']
    bfc= shape['head_width']
    tfc= 0.75*shape['dimE'] # rough estimation.
    bft= shape['foot_width']
    tft= 0.8*shape['dimF'] # rough estimation.
    h= D+(tft+tfc)/2.0
    alfa= 1.0/(1+(bfc/bft)**3*(tfc/tft))
    return h**2*bfc**3*tfc*alfa/12.0

def estimate_st_venant_torsion_constant(shape):
    ''' Return an estimation of the St. Venant torsion constant according to 
        the equation (17) of the article:

     Simplified Lateral Torsional Buckling Equations for Singly-Symmetric 
     I-Section Members of Donald W. White and Se-Kwon Jung.

    :param shape: shape to compute the warping constant for.
    '''
    D= shape['h']
    bfc= shape['head_width']
    tfc= 0.75*shape['dimE'] # rough estimation.
    bft= shape['foot_width']
    tft= 0.8*shape['dimF'] # rough estimation.
    h= D+(tft+tfc)/2.0
    tw= shape['tw']
    retval= D*tw**3+bfc*tfc**3*(1-0.63*tfc/bfc)+bft*tft**3*(1-0.63*tft/bft)
    retval/=3.0
    return retval

def estimate_avz(shape):
    ''' Return a rough estimation of the portion of the section that is
    subjected to shear forces in Z direction. 

    :param shape: shape to compute the warping constant for.
    '''
    bfc= shape['head_width']
    tfc= 0.75*shape['dimE'] # rough estimation.
    bft= shape['foot_width']
    tft= 0.8*shape['dimF'] # rough estimation.
    return bfc*tfc+bft*tft

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
        shapeRecord['hi']= shapeRecord['h']-shapeRecord['dimE']-shapeRecord['dimF']
        shapeRecord['Avy']= shapeRecord['tw']*shapeRecord['hi']
        shapeRecord['Avz']= estimate_avz(shapeRecord)
        shapeRecord['Cw']= estimate_warping_constant(shapeRecord)
        shapeRecord['It']= estimate_st_venant_torsion_constant(shapeRecord)
        retval[shapeName]= shapeRecord
    return retval

namePrefix= 'RAIL_'

shapesDict= su.read_shapes_from_spreadsheet(namePrefix= namePrefix, columnOrder= columnOrder, columnUnits= columnUnits, spreadsheetFileName= fNameIn, E= 210000e6, nu= 0.3, post_processing_function= post_process)

jsonFile= open(fNameOut, "w")
jsonFile.write(json.dumps(shapesDict))
jsonFile.close()
