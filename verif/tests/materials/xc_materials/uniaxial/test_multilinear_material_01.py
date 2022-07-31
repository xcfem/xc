# -*- coding: utf-8 -*-
''' Trivial test for MultiLinear material.'''
from __future__ import print_function
fy= 2600 # Yield stress of the material.
E= 2.1e6 # Young modulus of the material.
Nsteps= 10 # number of analysis steps.

import math
import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Define materials
sampleStrains= [0.02, 0.05, 0.1, 0.270833333]
sampleStresses= [13527, 15957, 16100, 16100.41376]
sampleStrainStressCurve= list()
for strain, stress in zip(sampleStrains, sampleStresses):
    sampleStrainStressCurve.append((strain, stress))
mlinear= typical_materials.defMultiLinearMaterial(preprocessor, "mlinear", sampleStrainStressCurve)

numIntervals= mlinear.numPoints
ratio= numIntervals-len(sampleStrainStressCurve)

# Compute stresses.
err= 0.0
stresses= list()
for ss in sampleStrainStressCurve:
    sampleStrain= ss[0]
    mlinear.setTrialStrain(sampleStrain, 0.0)
    mlinear.commitState()
    stress= mlinear.getStress()
    sampleStress= ss[1]
    err+= (sampleStress-stress)**2
    stresses.append(stress)

err= math.sqrt(err)

#print(numPoints)
#print(err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio==0 and err<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # FIGURES & REPORTS
# import matplotlib.pyplot as plt
# plt.title(fname)
# plt.xlabel('strain')
# plt.ylabel('stress')
# plt.plot(sampleStrains, stresses, color= 'blue')
# plt.plot(sampleStrains, sampleStresses, 'ro')
# plt.show()
