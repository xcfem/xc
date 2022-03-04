# -*- coding: utf-8 -*-
''' Simply supported beam-column capacity check example E1.7
    taken from the document NDS Structural Wood Design
    Examples 2015/2018 Edition.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2022, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import math
import xc_base
import geom
import xc

from materials.sections import section_properties
from materials.awc_nds import AWCNDS_materials
from materials.awc_nds import AWCNDS_limit_state_checking
from materials.awc_nds import dimensional_lumber

inchToMeter= 2.54/100.0
footToMeter= 0.3048
psiToPa= 6894.76
psfToPa= 47.88026
poundToNewton= 4.4482216282509

def getLoadCombDurationFactor(loadComb):
    dl= ('DL' in loadComb)
    sl= ('SL' in loadComb)
    wl= ('WL' in loadComb)
    return AWCNDS_materials.getLoadCombinationDurationFactor(deadLoad= dl, snowLoad= sl, windLoad= wl)

height= 9*footToMeter

wood= dimensional_lumber.SouthernPineWood(name='SouthernPine', grade= 'no_1', sub_grade= '')
columnSection= AWCNDS_materials.DimensionLumberSection(name= '2x6', woodMaterial= wood)

column= AWCNDS_limit_state_checking.ColumnMember(unbracedLengthB= .01*height, unbracedLengthH= height, section= columnSection, memberRestraint= AWCNDS_materials.MemberRestraint.compressionEdgeSupport)

# Member dimensions and properties
w_trib= 4.0*footToMeter
Ag= column.section.A()
S= column.section.getElasticSectionModulusZ() 
I= column.section.Iz()
Emin= column.section.wood.getEminAdj()

## Effective lengths.
leH, leB= column.getColumnEffectiveLength() 
ratio1= abs(leH-2.7432000000000003)/2.7432000000000003
## Critical buckling design values for compression.
FcE1= column.getFcE1()
ratio2= abs(FcE1-8527263.374485593)/8527263.374485593
## Slenderness ratio for bending as column.
RBH= column.getColumnBendingSlendernessRatio()
ratio3= abs(RBH-16.24807680927192)/16.24807680927192
## Critical buckling design values for bending.
FbE= column.getFbECriticalBucklingDesignValue()
ratio4= abs(FbE-10198589.195161333)/10198589.195161333

# Loads
DL= xc.Vector([0.0, -560*poundToNewton])
SL= xc.Vector([0.0, -840*poundToNewton])
WL= xc.Vector([-25*psfToPa*w_trib, 0.0])

# Load cases
loadCases= dict()
loadCases['LC1']= 'DL+SL+WL'
loadCases['LC2']= 'DL+SL'
loadCases['LC3']= 'DL'
loadCases['LC4']= 'DL+SL-WL' # Make sure sign is duly considered.

results= dict()

for key in loadCases:
    results[key]= dict()
    loadCase= loadCases[key]
    wood.CD= getLoadCombDurationFactor(loadCase)
    results[key]['CD']= wood.CD
    # Compression
    Fc_aster= column.section.getFcAdj()
    results[key]['Fc*']= Fc_aster/psiToPa
    ## Column stability factor.
    CP= column.getColumnStabilityFactor()
    results[key]['CP']= CP
    Fc_aster2= column.getFcAdj()
    results[key]['Fc**']= Fc_aster2/psiToPa
    # Bending
    CL= column.getBeamStabilityFactor()
    results[key]['CL']= CL
    Fb_aster2= column.getFbAdj()
    results[key]['Fb**']= Fb_aster2/psiToPa
    # Bending and axial compression.
    loadVector= eval(loadCase)
    C= loadVector[1] # Compression
    M= loadVector[0]*height**2/8.0 # Bending moment.
    bendingCF= column.getBiaxialBendingEfficiency(Nd= C, Myd= 0.0, Mzd= M, chiN= CP, chiLT= CL)
    results[key]['efficiency']= bendingCF[0]
    
refResults= {'LC1': {'CD': 1.6, 'Fc*': 2483.522866299488, 'CP': 0.43219668428603586, 'Fc**': 1073.3703481631908, 'CL': 1.0, 'Fb**': 2158.843220409839, 'efficiency': 0.887544474580459}, 'LC2': {'CD': 1.15, 'Fc*': 1785.032060152757, 'CP': 0.5546795481910816, 'Fc**': 990.1207766321268, 'CL': 1.0, 'Fb**': 1551.6685646695717, 'efficiency': 0.17139010469812982}, 'LC3': {'CD': 0.9, 'Fc*': 1396.981612293462, 'CP': 0.6474764318198925, 'Fc**': 904.5126696457712, 'CL': 1.0, 'Fb**': 1214.3493114805344, 'efficiency': 0.07504456676642514}, 'LC4': {'CD': 1.6, 'Fc*': 2483.522866299488, 'CP': 0.43219668428603586, 'Fc**': 1073.3703481631908, 'CL': 1.0, 'Fb**': 2158.843220409839, 'efficiency': 0.887544474580459}}

error= 0.0
for lc in results:
    combResults= results[lc]
    for key in combResults:
        v1= combResults[key]
        v2= refResults[lc][key]
        error+= (v1-v2)**2

error= math.sqrt(error)

'''
print('error: ', error)
print(results)
print('Minimum value of adjusted elastic modulus: '+'{:.2f}'.format(Emin/1e6)+' MPa ('+'{:.2f}'.format(Emin/psiToPa)+' psi).')
print('Effective length H plane: '+'{:.2f}'.format(leH)+' m ('+'{:.2f}'.format(leH/inchToMeter)+' in).')
print('ratio1= ', ratio1)
print('Effective length B plane: '+'{:.2f}'.format(leB)+' m ('+'{:.2f}'.format(leB/inchToMeter)+' in).')
print('Critical buckling design value for compression member: '+'{:.2f}'.format(FcE1/1e6)+' MPa ('+'{:.2f}'.format(FcE1/psiToPa)+' psi).')
print('ratio2= ', ratio2)
print('Bending slenderness ratio: '+'{:.2f}'.format(RBH))
print('ratio3= ', ratio3)
print('Critical buckling design value for bending: '+'{:.2f}'.format(FbE/1e6)+' MPa ('+'{:.2f}'.format(FbE/psiToPa)+' psi).')
print('ratio4= ', ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-10) and (ratio2<1e-10) and (ratio3<1e-10) and (ratio4<1e-10) and (error<1e-10)):
    print("test "+fname+": ok.")
else:
    lmsg.error("test "+fname+": ERROR.")
