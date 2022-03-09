# -*- coding: utf-8 -*-
''' Example based on chapter 5 of the document:
    ASD Wood I-Joists guideline of the American Wood Council
    1999 Edition.'''

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
from materials.awc_nds import pr_400_i_joists

inchToMeter= 2.54/100.0
footToMeter= .3048
psiToPa= 6894.76
psfToPa= 47.88026
poundToNewton= 4.4482216282509

# Geometry
span= 18*footToMeter+10*inchToMeter
w_trib= 1.33*footToMeter

# Materials
beamSection= pr_400_i_joists.pr400_i_joists['PRI-20_302']
beam= AWCNDS_limit_state_checking.BeamMember(unbracedLength= span, section= beamSection)

# Beam Stability Factor.
le= beam.getEffectiveLength() # Effective length
RB= beam.getBendingSlendernessRatio()
FbE= beam.getFbECriticalBucklingDesignValue()

# Member dimensions and properties
Ag= beam.crossSection.A()
S= beam.crossSection.getElasticSectionModulusZ() 
I= beam.crossSection.Iz()

# Loads
DL= 10*psfToPa*w_trib
LL= 40*psfToPa*w_trib

# Load cases
loadCases= dict()
loadCases['LC1']= 'DL+LL'
loadCases['LC2']= 'DL'

results= dict()

for key in loadCases:
    results[key]= dict()
    loadCase= loadCases[key]
    load= eval(loadCase)
    results[key]['w']= load/poundToNewton*footToMeter
    # Tension
    T= 0.0
    ft= T/Ag # Tensile stress in bottom chord
    results[key]['ft']= ft/psiToPa
    Ft_aster= beam.getFtAdj()
    results[key]['Ft*']= Ft_aster/psiToPa
    # Bending
    M= (load*span**2)/8.0
    results[key]['Mzd']= M/footToMeter/poundToNewton
    Fb_aster= beam.crossSection.getFbAdj()
    results[key]['Fb*']= Fb_aster/psiToPa
    CL= 1.0 #beam.getBeamStabilityFactor()
    results[key]['CL']= CL
    Fb_aster2= beam.getFbAdj()
    results[key]['Fb**']= Fb_aster2/psiToPa
    # Bending and axial tension.
    bendingCF= beam.getBiaxialBendingEfficiency(Nd= T, Myd= 0.0, Mzd= M, chiN= 1.0, chiLT= CL)
    results[key]['bendingCF']= bendingCF[0]
    # Shear
    V= load*span/2.0
    results[key]['Vyd']= V/poundToNewton
    shearCF= beam.crossSection.getYShearEfficiency(V)
    results[key]['shearCF']= shearCF

refResults= {'LC1': {'w': 66.50000122198874, 'ft': 0.0, 'Ft*': 246.56405734209747, 'Mzd': 2948.397623623522, 'Fb*': 1604.5632545931758, 'CL': 1.0, 'Fb**': 827.1383261124746, 'bendingCF': 0.9030314371666026, 'Vyd': 626.208344840394, 'shearCF': 0.4409917949418857}, 'LC2': {'w': 13.300000244397749, 'ft': 0.0, 'Ft*': 246.56405734209747, 'Mzd': 589.6795247247044, 'Fb*': 1604.5632545931758, 'CL': 1.0, 'Fb**': 827.1383261124746, 'bendingCF': 0.18060628743332055, 'Vyd': 125.24166896807881, 'shearCF': 0.08819835898837715}}
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
