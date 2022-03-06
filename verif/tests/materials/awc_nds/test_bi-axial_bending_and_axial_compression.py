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

length= 3*footToMeter

wood= dimensional_lumber.SouthernPineWood(name='SouthernPine', grade= 'no_2', sub_grade= '')
columnSection= AWCNDS_materials.DimensionLumberSection(name= '2x4', woodMaterial= wood)
loadingCondition= AWCNDS_materials.MemberLoadingCondition(numberOfConcentratedLoads= 1, lateralSupport= False, cantilever= False)
column= AWCNDS_limit_state_checking.ColumnMember(unbracedLengthB= length, unbracedLengthH= length, section= columnSection, memberLoadingCondition= loadingCondition)

# Member dimensions and properties
Ag= column.crossSection.A()
Sz= column.crossSection.getElasticSectionModulusZ() 
Sy= column.crossSection.getElasticSectionModulusY() 
Iz= column.crossSection.Iz()
Iy= column.crossSection.Iy()
Emin= column.crossSection.wood.getEminAdj()

## Effective lengths.
leH, leB= column.getBeamEffectiveLength()
ratio1= abs(leH-1.519428)/1.519428
## Critical buckling design values for compression.
(FcE1, FcE2)= column.getFcE()
ratio2= math.sqrt((FcE1-27193865.740740728)**2+(FcE2-4994791.666666663)**2)

## Slenderness ratio for bending.
(RBH, RBB)= column.getBeamBendingSlendernessRatioHB()
ratio3= math.sqrt((RBH-9.64641556918078)**2+(RBB-2.706454869230905)**2)

## Critical buckling design values for bending.
FbE= column.getFbECriticalBucklingDesignValue()
ratio4= abs(FbE-45135406.21865596)/45135406.21865596

# Loads (components: [weakAxisLoad, strongAxisLoad, axialLoad])
DL= xc.Vector([50.0, 0.0, -300.0])*poundToNewton
SL= xc.Vector([100.0, 0.0,-600.0])*poundToNewton
WL= xc.Vector([0.0, 120, 0.0])*poundToNewton

# Load cases
loadCases= dict()
loadCases['LC1']= 'DL+SL+WL'
loadCases['LC2']= 'DL+SL'
loadCases['LC3']= 'DL'
loadCases['LC4']= 'DL+SL-WL'

results= dict()

for key in loadCases:
    results[key]= dict()
    loadCase= loadCases[key]
    wood.CD= getLoadCombDurationFactor(loadCase)
    results[key]['CD']= wood.CD
    # Compression
    Fc_aster= column.crossSection.getFcAdj()
    results[key]['Fc*']= Fc_aster/psiToPa
    ## Column stability factor.
    CP= column.getColumnStabilityFactor()
    results[key]['CP']= CP
    Fc_aster2= column.getFcAdj()
    results[key]['Fc**']= Fc_aster2/psiToPa
    # Bending
    CL1= column.getBeamStabilityFactor(majorAxis= True)
    CL2= column.getBeamStabilityFactor(majorAxis= False)
    results[key]['CL1']= CL1
    results[key]['CL2']= CL2
    Fb1_aster2= column.getFbAdj(majorAxis= True)
    Fb2_aster2= column.getFbAdj(majorAxis= False)
    results[key]['Fb1**']= Fb1_aster2/psiToPa
    results[key]['Fb2**']= Fb2_aster2/psiToPa
    # Bending and axial compression.
    loadVector= eval(loadCase)
    C= loadVector[2] # Compression
    fc= C/column.crossSection.A()
    results[key]['fc']= abs(fc)/psiToPa
    My= loadVector[0]*length/4.0 # Bending moment (weak axis).
    fb2= abs(My)/column.crossSection.getElasticSectionModulusY()
    results[key]['fb2']= fb2/psiToPa
    Mz= loadVector[1]*length/4.0 # Bending moment (strong axis).
    fb1= abs(Mz)/column.crossSection.getElasticSectionModulusZ()
    results[key]['fb1']= fb1/psiToPa
    bendingCF= column.getBiaxialBendingEfficiency(Nd= C, Myd= My, Mzd= Mz, chiN= CP, chiLT= min(CL1, CL2))
    results[key]['efficiency']= bendingCF[0]

refResults= {'LC1': {'CD': 1.6, 'Fc*': 2320, 'CP': 0.29, 'Fc**': 673, 'CL1': 0.982, 'CL2': 1.0, 'Fb1**': 1729, 'Fb2**': 1936, 'fc':171, 'fb2':1029, 'fb1':353, 'efficiency': 0.98},
             'LC2': {'CD': 1.15, 'Fc*': 1667, 'CP': 0.387, 'Fc**': 646, 'Fb2**': 1392,'fc':171, 'fb2':1029, 'fb1':0.0, 'efficiency': 1.04},
             'LC3': {'CD': 0.9, 'Fc*': 1305, 'CP': 0.473, 'Fc**': 617, 'Fb2**': 1089,'fc':57, 'fb2':353, 'fb1':0.0, 'efficiency': 0.35},
             'LC4': {'CD': 1.6, 'CP': 0.29, 'Fc**': 673, 'CL1': 0.982, 'CL2': 1.0, 'Fb1**': 1729, 'Fb2**': 1936, 'fc':171, 'fb2':1029, 'fb1':353, 'efficiency': 0.98}}

error= 0.0
maxErroLoadComb= None
maxErrorKey= None
for lc in results:
    combResults= results[lc]
    for key in combResults:
        v1= combResults[key]
        if(key in refResults[lc]):
            v2= refResults[lc][key]
            #error+= abs(v1-v2)/abs(v2)
            keyError= abs(v1-v2)
            if(abs(v2)>0.0):
                keyError/=abs(v2)
            if(keyError>error):
                error= keyError
                maxErrorLoadComb= lc
                maxErrorKey= key
        #error/=len(refResults[lc]) # average error

'''
print('error: ', error, 'load combination: ', maxErrorLoadComb, 'value: ', maxErrorKey)
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
if((ratio1<1e-10) and (ratio2<1e-10) and (ratio3<1e-10) and (ratio4<1e-10) and (error<0.07)):
    print("test "+fname+": ok.")
else:
    lmsg.error("test "+fname+": ERROR.")
