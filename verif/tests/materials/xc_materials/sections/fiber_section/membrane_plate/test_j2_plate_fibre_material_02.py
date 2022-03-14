# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2020, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmail.com"

import math
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from misc_utils import units_utils

# Data

width=1*units_utils.inchToMeter   # Rod width (m)
thickness=1*units_utils.inchToMeter   # Rod thickness (m)
L= 50*units_utils.inchToMeter # Length of the rod (m)

E= 29000*units_utils.ksiToPa # Young’s modulus (Pa)

nu= 0.3 # Poisson’s ratio
sg_yield= 60*units_utils.ksiToPa # Allowable stress: yield stress of steel (Pa)
alpha=0.05  # strain hardening ratio

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)


# Material definition
j2plate= typical_materials.defJ2PlateFibre(preprocessor, "j2plate",E,nu,fy= sg_yield, alpha=alpha,rho= 7850.0)


eps_yield= sg_yield/E
strainData= [3e-4, 7e-4, 1e-3, 2e-3, eps_yield, 2.05e-3, 2.1e-3, 2.25e-3, 2.5e-3, 3e-3, 4e-3, 5e-3, 6e-3, 7e-3]
stressData= list()
stressDataReference= [65916936.263736255, 153806184.6153846, 219723120.8791209, 439446241.7582418, 454599381.9692308, 450432397.8021978, 461418553.8461538, 468395275.69213086, 473603340.7125834, 482821613.0226728, 498619090.5284576, 512957947.0980535, 526644627.1041995, 540040452.9319803]

for strain in strainData:
    j2plate.setTrialStrain(xc.Vector([strain,0,0,0,0]))
    j2plate.commitState()
    stress= j2plate.getStress()
    sigma_11= stress[0]
    sigma_22= stress[1]
    stressData.append(sigma_11)
    


err= 0.0
for sg,sgRef in zip(stressData,stressDataReference):
    diff= (sg-sgRef)/1e6
    err+= diff**2

err= math.sqrt(err)

'''
print('err= ', err)
print(stressData)
print('sg_yield= ', sg_yield/1e6, ' MPa')

import matplotlib.pyplot as plt
plt.plot(strainData, stressData)
plt.ylabel('stress-strain')
plt.show()
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-3):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

