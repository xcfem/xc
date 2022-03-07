# -*- coding: utf-8 -*-
''' Simply supported beam capacity check example E1.6
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
footToMeter= .3048
psiToPa= 6894.76
psfToPa= 47.88026
poundToNewton= 4.4482216282509

def getLoadCombDurationFactor(loadComb):
    dl= ('DL' in loadComb)
    cl= ('RLL' in loadComb)
    sl= ('SL' in loadComb)
    wl= ('WL' in loadComb)
    return AWCNDS_materials.getLoadCombinationDurationFactor(deadLoad= dl, constructionLoad= cl, snowLoad= sl, windLoad= wl)

# Materials
# HemFir No. 2.
wood= dimensional_lumber.HemFirWood(grade= 'no_2')

beamSection= AWCNDS_materials.CustomLumberSection("HemFir", b=1.5*inchToMeter,h=7.25*inchToMeter, woodMaterial= wood, Cr= 1.0)

length= 12.0*footToMeter
beam= AWCNDS_limit_state_checking.BeamMember(unbracedLength= length, section= beamSection)
Fb= beam.crossSection.wood.Fb
Ft= beam.crossSection.wood.Ft
E= beam.crossSection.wood.E
Emin= beam.crossSection.wood.Emin
Fct= beam.crossSection.wood.Fct
Fv= beam.crossSection.wood.Fv

# Beam Stability Factor.
le= beam.getEffectiveLength() # Effective length
RB= beam.getBendingSlendernessRatio()
FbE= beam.getFbECriticalBucklingDesignValue()

Cfu= 1.0
CM= 1.0
# Cr= 1.0 See section constructor.
CT= 1.0
Ct= 1.0
Ci= 1.0
CF= beam.crossSection.getBendingSizeFactor() #1.2 # (NDS Table 4.3.1)
refCF= 1.2
ratio0= abs(CF-refCF)/refCF

Ep= E*CM*Ct*Ci
Epmin= Emin*CM*Ct*Ci*CT

# Member dimensions and properties
w_trib= 4.0*footToMeter
Ag= beam.crossSection.A()
S= beam.crossSection.getElasticSectionModulusZ() 
I= beam.crossSection.Iz()

# Loads
WL= xc.Vector([880.0*poundToNewton,0.0])
RLL= xc.Vector([880.0*poundToNewton,0.0])
DL= xc.Vector([1420.0*poundToNewton,8*psfToPa*w_trib])

# Load cases
loadCases= dict()
loadCases['LC1']= 'DL+RLL+WL'
loadCases['LC2']= 'DL+RLL'
loadCases['LC3']= 'DL'

results= dict()

for key in loadCases:
    results[key]= dict()
    loadCase= loadCases[key]
    wood.CD= getLoadCombDurationFactor(loadCase)
    results[key]['CD']= wood.CD
    loadVector= eval(loadCase)
    # Tension
    T= loadVector[0]
    ft= T/Ag # Tensile stress in bottom chord
    results[key]['ft']= ft/psiToPa
    refFt_aster= Ft*wood.CD*CM*Ct*CF*Ci
    Ft_aster= beam.getFtAdj()
    results[key]['Ft*']= Ft_aster/psiToPa
    # Bending
    #refFb_aster= Fb*wood.CD*CM*Ct*CF*Ci*Cr
    M= loadVector[1]*length**2/8.0
    Fb_aster= beam.crossSection.getFbAdj()
    results[key]['Fb*']= Fb_aster/psiToPa
    CL= beam.getBeamStabilityFactor()
    results[key]['CL']= CL
    Fb_aster2= beam.getFbAdj()
    results[key]['Fb**']= Fb_aster2/psiToPa
    # Bending and axial tension.
    bendingCF= beam.getBiaxialBendingEfficiency(Nd= T, Myd= 0.0, Mzd= M, chiN= 1.0, chiLT= CL)
    results[key]['CF']= bendingCF[0]

refResults= {'LC1': {'CD': 1.6, 'ft': 292.4136791579146, 'Ft*': 1008.0, 'Fb*': 1632.0, 'CL': 0.40444999015184435, 'Fb**': 660.06238392781, 'CF': 0.6123981947524744}, 'LC2': {'CD': 1.25, 'ft': 211.49417046012692, 'Ft*': 787.5, 'Fb*': 1275.0, 'CL': 0.5089065816672449, 'Fb**': 648.8558916257373, 'CF': 0.6811147576034369}, 'LC3': {'CD': 0.9, 'ft': 130.57466176233925, 'Ft*': 567.0, 'Fb*': 918.0000000000001, 'CL': 0.6738331918478999, 'Fb**': 618.5788701163722, 'CF': 0.8032775360051476}}

error= 0.0
for lc in results:
    combResults= results[lc]
    for key in combResults:
        v1= combResults[key]
        v2= refResults[lc][key]
        error+= (v1-v2)**2

error= math.sqrt(error)

'''
print('results: ', results)
print('error: ', error)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(error<1e-10):
    print("test "+fname+": ok.")
else:
    lmsg.error("test "+fname+": ERROR.")
