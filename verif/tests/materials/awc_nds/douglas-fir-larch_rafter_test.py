# -*- coding: utf-8 -*-
''' Home made test based on Example 5.7 from the book:
    Residential Structural Design Guide. Second edition. October 2017'''

 
from __future__ import division
from __future__ import print_function

import math
from materials.awc_nds import AWCNDS_materials
from materials.awc_nds import AWCNDS_limit_state_checking
from materials.awc_nds import dimensional_lumber
from materials.sections import section_properties as sp
from rough_calculations import ng_simple_beam as sb

__author__= "Luis Claudio Pérez Tato (LCPT"
__copyright__= "Copyright 2015, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

inch2meter= 0.0254
pound2Newton= 4.4482216282509
foot2meter= 0.3048
psf2Pa= 47.88026

def getLoadCombDurationFactor(loadComb):
    dl= ('D' in loadComb)
    cl= ('RL' in loadComb)
    sl= ('S' in loadComb)
    wl= ('W' in loadComb)
    return AWCNDS_materials.getLoadCombinationDurationFactor(deadLoad= dl, constructionLoad= cl, snowLoad= sl, windLoad= wl)

# Geometry
span= 14.4*foot2meter
w_trib= 16*inch2meter
cosAlpha= math.cos(math.radians(33.7))

# Materials
wood= dimensional_lumber.DouglasFirLarchWood(name='DouglasFirLarch', grade= 'no_2')
rafterSection= AWCNDS_materials.DimensionLumberSection(name= '2x8', woodMaterial= wood)
rafter= AWCNDS_limit_state_checking.BeamMember(unbracedLength= span, section= rafterSection, Cr= 1.15)
rafterModel= sb.SimpleBeam(E= wood.E,I= rafterSection.Iz(),l= span)

# Loads
D= -10.0*w_trib*psf2Pa*cosAlpha # Transverse dead load.
S= -20.0*w_trib*psf2Pa*cosAlpha # Transverse snow load.
Wu= 12.7*w_trib*psf2Pa # Transverse wind load (outward, uplift).
W= -7.4*w_trib*psf2Pa # Transverse wind load (inward).
RL= 10*w_trib*psf2Pa # Transverse roof live load.

# Load cases
loadCases= dict()
loadCases['LC1']= 'D+RL'
loadCases['LC2']= 'D+S'
loadCases['LC3']= '0.6*D+Wu'
loadCases['LC4']= 'D+W'

results= dict()
maxCF= 0.0
# Check bending stress for both loading cases and bending conditions.
for key in loadCases:
    loadCase= loadCases[key]
    results[key]= dict()
    wood.CD= getLoadCombDurationFactor(loadCase)
    loadValue= eval(loadCase)
    Mz= rafterModel.getBendingMomentUnderUniformLoad(q= loadValue, x= span/2.0)
    Vy= rafterModel.getShearUnderUniformLoad(q= loadValue, x=0.0)
    CL= 1.0 # Compression edge support.
    if(loadValue>0.0): # but not when load is outwards.
        CL= rafter.getBeamStabilityFactor()
    results[key]['CL']= CL
    bendingCF= rafter.getBiaxialBendingEfficiency(Nd= 0.0, Myd= 0.0, Mzd= Mz, chiN= 1.0, chiLT= CL)
    results[key]['efficiency']= bendingCF[0]
    maxCF= max(bendingCF[0], maxCF)
    
refResults= {'LC1': {'CL': 0.4411390755209422, 'efficiency': 0.0774389619989348}, 'LC2': {'CL': 1.0, 'efficiency': 0.5514929858165736}, 'LC3': {'CL': 0.3488943131389552, 'efficiency': 0.3508815580900423}, 'LC4': {'CL': 1.0, 'efficiency': 0.24965317014828287}}

error= 0.0
for lc in results:
    combResults= results[lc]
    for key in combResults:
        v1= combResults[key]
        v2= refResults[lc][key]
        error+= (v1-v2)**2

# Check deflection.
designLoad= eval(loadCases['LC2'])
delta= -rafterModel.getDeflectionUnderUniformLoad(designLoad,rafterModel.l/2.0)
ratio= abs(delta-10.729547227761856e-3)/10.729547227761856e-3

'''
print('delta= ', delta*1000, 'mm (',delta/inch2meter,'in)')
print('ratio= ',ratio)
print(results)
print(maxCF)
print(error)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((abs(error)<1e-10) and (abs(ratio)<1e-10)):
    print('test '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
