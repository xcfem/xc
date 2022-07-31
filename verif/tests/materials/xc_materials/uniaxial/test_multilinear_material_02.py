# -*- coding: utf-8 -*-
''' Trivial test for MultiLinear material.'''
from __future__ import print_function
fy= 2600 # Yield stress of the material.
E= 2.1e6 # Young modulus of the material.
Nsteps= 10 # number of analysis steps.

import math
import numpy as np
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
sampleStrainStressCurve= [(0.02, 13527), (0.05, 15957), (0.1, 16100), (0.270833333, 16100.41376)]
sampleStrains= list()
sampleStresses= list()
for value in sampleStrainStressCurve:
    sampleStrains.append(value[0])
    sampleStresses.append(value[1])
maxStrain= sampleStrains[-1]
mlinear= typical_materials.defMultiLinearMaterial(preprocessor, "1005", sampleStrainStressCurve)

strainValuesUp= list([0.0])
startStrain= 0.0
for strain in sampleStrains:
    values= np.linspace(start= startStrain, stop= strain, num= 5)
    strainValuesUp.extend(values.tolist()[1:])
    startStrain= strain

strainValuesDown= strainValuesUp[::-1]
for strain in strainValuesUp[1:]:
    strainValuesDown.append(-strain)


#strainValuesDown= np.linspace(start= maxStrain, stop= -maxStrain, num= 10)
strains= strainValuesUp
strains.extend(strainValuesDown[1:])
strains.extend(strainValuesDown[::-1]) # up again.


numIntervals= mlinear.numPoints
ratio= numIntervals-len(sampleStrainStressCurve)

# Compute stresses.
stresses= list()
for strain in strains:
    mlinear.setTrialStrain(strain, 0.0)
    mlinear.commitState()
    stress= mlinear.getStress()
    stresses.append(stress)

refStresses= [0.0, 3381.750000000001, 6763.499999999999, 10145.250000000002, 13527.0, 14134.5, 14742.0, 15349.5, 15957.0, 15992.75, 16028.5, 16064.25, 16100.0, 16100.10344, 16100.20688, 16100.310319999999, 16100.41376, -11172.96123325, -14632.336226500001, -15894.023739285001, -16016.16957238, -16051.91957238, -16087.66957238, -16099.606423413867, -16099.636698535876, -16099.654863609083, -16099.67302868229, -16099.691193755496, -16099.7093588287, -16099.721468877506, -16099.73357892631, -16099.745688975114, -16099.757799023919, -16099.769909072722, -16099.782019121527, -16099.79412917033, -16099.806239219135, -16099.82440429234, -16099.842569365546, -16099.860734438753, -16099.87889951196, -16099.909174633969, -16099.939449755979, -16099.96972487799, -16100.0, -16100.10344, -16100.20688, -16100.310319999999, -16100.41376, -16100.41376, 11172.96123325, 14632.336226500001, 15894.023739285001, 16016.16957238, 16051.91957238, 16087.66957238, 16099.606423413867, 16099.636698535876, 16099.654863609083, 16099.67302868229, 16099.691193755496, 16099.7093588287, 16099.721468877506, 16099.73357892631, 16099.745688975114, 16099.757799023919, 16099.769909072722, 16099.782019121527, 16099.79412917033, 16099.806239219135, 16099.82440429234, 16099.842569365546, 16099.860734438753, 16099.87889951196, 16099.909174633969, 16099.939449755979, 16099.96972487799, 16100.0, 16100.10344, 16100.20688, 16100.310319999999, 16100.41376]

# Compute stresses.
err= 0.0
for ss, ssRef in zip(stresses, refStresses):
    err+= (ss-ssRef)**2

err= math.sqrt(err)

#print(numPoints)
# print(strains)
# print(stresses)
# print(err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio==0 and err<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # FIGURES & REPORTS
# import matplotlib.pyplot as plt
# plt.title(fname)
# plt.xlabel('index')
# plt.ylabel('strain')
# xi= range(0, len(strains))
# plt.plot(xi, strains, color= 'blue')
# plt.show()

# plt.xlabel('strain')
# plt.ylabel('stress')
# plt.plot(strains,stresses, color= 'blue')
# plt.plot(sampleStrains, sampleStresses, 'ro')
# plt.show()
